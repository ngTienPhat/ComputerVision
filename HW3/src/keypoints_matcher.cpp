#include "keypoints_matcher.hpp"

using namespace MatrixHelper;

// ---------------------------------------------------------------------
void KeypointsMatcher::knnMatchTwoImages(const string& imageTrain, const string& imageTest){
    Sift siftModel1(siftBaseSigma, siftNumOctaves, siftNumDOGperOctave);
    vector<Extrema> trainKeypoints = siftModel1.extractKeypoints(imageTrain);

    Sift siftModel2(siftBaseSigma, siftNumOctaves, siftNumDOGperOctave);
    vector<Extrema> testKeypoints = siftModel2.extractKeypoints(imageTest);


    for(int j = 0; j < siftNumOctaves; j++){
        vector<KeyPoint> kp_train, kp_test;
        vector<Mat> descriptors_train, descriptors_test;

        vector<KeyPoint> trainKp, testKp;
        Mat trainDescrip, testDescrip;

        createInputForKNNmatcher(trainKeypoints, trainDescrip, trainKp, j);
        createInputForKNNmatcher(testKeypoints, testDescrip, testKp, j);

        Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("BruteForce");
        vector<vector<DMatch>> matches;
        vector<DMatch> goodMatches;
        int k=2;

        cout << "------ octave " << j << " -----------" << endl;
        cout << "start matching points" << endl;
        cout << "train keypoint: " << trainKp.size() << endl;
        cout << "test keypoint: " << testKp.size() << endl;
        cout << "train descrip matrix: "; MatrixHelper::printMatrixInfo(trainDescrip);
        cout << "test descrip matrix: "; MatrixHelper::printMatrixInfo(testDescrip);

        matcher->knnMatch(testDescrip, trainDescrip, matches, k);
        cout << "matches: " << matches.size() << endl;

        // thresholding result
        const double ratio = 0.8;
        for(int i = 0; i < matches.size(); i++){
            if (matches[i].size() > 1){
                // As in Lowe's paper; can be tuned
                //cout << matches[i][0].distance << endl;
                if (matches[i][0].distance < ratio * matches[i][1].distance)
                    goodMatches.push_back(matches[i][0]);
            }
            else if (matches[i].size() == 1)
                goodMatches.push_back(matches[i][0]);
        }
        cout << "good matches: " << goodMatches.size() << endl;

        Mat trainImage = imread(imageTrain, IMREAD_COLOR);
        Mat testImage = imread(imageTest, IMREAD_COLOR);
        
        resize(trainImage, trainImage, cv::Size(), pow(0.5, j), pow(0.5, j), INTER_NEAREST);
        resize(testImage, testImage, cv::Size(), pow(0.5, j), pow(0.5, j), INTER_NEAREST);
    
        cout << "train image size: "; MatrixHelper::printMatrixInfo(trainImage);
        cout << "test image size: "; MatrixHelper::printMatrixInfo(testImage);

        Mat imgMatches;
        drawMatches(testImage, testKp, trainImage, trainKp, goodMatches, imgMatches, Scalar_<double>::all(-1), Scalar_<double>::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
        
        //-- Show detected matches
        string match_name = "Matching result-octave" + to_string(j);
        imshow(match_name, imgMatches);
    }

    waitKey(0);
}   

void KeypointsMatcher::createInputForKNNmatcher(const vector<Extrema> &myKeypoints, Mat& descriptors, vector<KeyPoint> &keypoints, int octaveIndex){
    int nKp = myKeypoints.size();
    int dimDescrip = myKeypoints[0].descriptors.size();
    vector<int> rawIndex;
    for(int i = 0; i < nKp; i++){
        if(myKeypoints[i].octaveIndex != octaveIndex)
            continue;
        KeyPoint kp;
        kp.pt = Point(myKeypoints[i].x, myKeypoints[i].y);
        keypoints.push_back(kp);
        rawIndex.push_back(i);
    }

    int nFinalKp = keypoints.size();
    descriptors = Mat::zeros(nFinalKp, dimDescrip, CV_32FC1);

    for(int i = 0; i < nFinalKp; i++){
        Extrema curRawKeypoint = myKeypoints[rawIndex[i]];
        for(int j = 0; j < dimDescrip; j++){
            setValueOfMatrix(descriptors, i, j, curRawKeypoint.descriptors[j]);
        }
    }

    //cout <<descriptors << endl;
}


// ---------------------------------------------------------------------
// PRIVATE AREA
Mat KeypointsMatcher::initTrainMatrix(const vector<Extrema> &keypoints){
    int nKp = keypoints.size();
    int lenDescriptor = keypoints[0].descriptors.size();
    Mat_<float> trainMat(nKp, lenDescriptor);
    
    for(int i = 0; i < nKp; i++){
        for(int j = 0; j < lenDescriptor; j++){
            setValueOfMatrix(trainMat, i, j, keypoints[i].descriptors[j]);
        }
    }

    return trainMat;
}

Mat KeypointsMatcher::getTrainLabels(const vector<Extrema> &keypoints){
    vector<int> keypointIndex;
    int nKps = keypoints.size();
    for(int i = 0; i < nKps; i++){
        keypointIndex.push_back(i);
    }

    Mat labels(1, nKps, CV_8UC1 , keypointIndex.data());
    return labels;
}

PairKeypoint KeypointsMatcher::getClosestKeypoint(const Mat &trainMatrix, const Mat& trainLabels, Extrema& testKp, const vector<Extrema>& listTrainKp){
    PairKeypoint res;
    res.pointTest = testKp;

    Ptr<ml::KNearest> knn(ml::KNearest::create());
    
    Mat descrip(1, testKp.descriptors.size(), CV_32FC1, testKp.descriptors.data());

    knn->train(trainMatrix, ml::ROW_SAMPLE, trainLabels);

    Mat response, dist;
    knn->findNearest(descrip, this->K, noArray(), response, dist);

    res.pointTrain = listTrainKp[getValueOfMatrix(response, 0, 0)];
    res.distance = getValueOfMatrix(dist, 0, 0);
    
    return res;
}

void KeypointsMatcher::visualizeMatchingResult(const vector<PairKeypoint> &result, const string& imageTrain, const string& imageTest){
    return;
}