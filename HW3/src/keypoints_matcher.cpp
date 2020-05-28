#include "keypoints_matcher.hpp"

using namespace MatrixHelper;

vector<PairKeypoint> KeypointsMatcher::matching(const string &imageTrain, const string& imageTest){
    vector<PairKeypoint> result;
    
    Sift siftModel(siftBaseSigma, siftNumOctaves, siftNumDOGperOctave);

    vector<Extrema> trainKeypoints = siftModel.extractKeypoints(imageTrain);
    vector<Extrema> testKeypoints = siftModel.extractKeypoints(imageTest);

    Mat trainMatrix = initTrainMatrix(trainKeypoints);

    int nTestKeypoints = testKeypoints.size();
    for(int i = 0; i < nTestKeypoints; i++){
        PairKeypoint nearestKeypoint = getClosestKeypoint(trainMatrix, getTrainLabels(trainKeypoints), testKeypoints[i], trainKeypoints);
        
        result.push_back(nearestKeypoint);
    }

    return result;
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