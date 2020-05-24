#include "blob_detector.hpp"

const float BlobDetector::k = sqrt(2);

void BlobDetector::detectBlob_LoG(const Mat& source){
    //convert to gray image and remove noise
    Mat grayImage;
    cvtColor(source, grayImage, COLOR_BGR2GRAY);
    Mat smoothenSource = OpencvHelper::applyGaussianKernel(grayImage, 1);

    //1. generate scale-normalized LoG kernels and use them to filter image
    vector<float> maxLogValues;
    vector<Mat> logImages = getScaleLaplacianImages(smoothenSource, maxLogValues);

    //2. find local maximum points
    vector<Blob> blobs = getLocalMaximumPoints(logImages, maxLogValues);

    //3. visualize result
    visualizeResult(source, blobs);
}

void BlobDetector::detectBlob_DoG(const Mat& source){
    //convert to gray image and remove noise
    Mat grayImage;
    cvtColor(source, grayImage, COLOR_BGR2GRAY);
    Mat smoothenSource = OpencvHelper::applyGaussianKernel(grayImage, 1);

    //1. generate scale-normalized LoG kernels and use them to filter image
    vector<float> maxLogValues;
    vector<Mat> logImages = getScaleLaplacianImages_DoG(smoothenSource, maxLogValues);

    //2. find local maximum points
    vector<Blob> blobs = getLocalMaximumPoints(logImages, maxLogValues);

    //3. visualize result
    visualizeResult(source, blobs);
}

// -----------------------------------------------------------------
// --------------- HELPER FUNCTIONS --------------------------------

vector<Mat> BlobDetector::getScaleLaplacianImages(const Mat& source, vector<float>& maxLogValues, float startSigma){
    vector<Mat> logImages;
    int nLayers=8;
    float sigma=startSigma;
    
    for(int i = 1; i <= nLayers; i++){
        float scaledSigma = sigma * pow(k, i);

        Mat logImage = calculateLoG(source, scaledSigma);

        // square log image
        logImage = MatrixHelper::applyOperator(logImage, logImage, "multiply");
        maxLogValues.push_back(MatrixHelper::getMaxValue(logImage));
        logImages.push_back(logImage);
    }

    return logImages;
}

vector<Mat> BlobDetector::getScaleLaplacianImages_DoG(const Mat& source, vector<float> &maxLogValues, float startSigma){
    vector<Mat> dogImages;
    int nLayers=8;
    float sigma=startSigma;

    Mat prevGauss = calculateGaussian(source, startSigma);

    for(int i = 2; i <= nLayers; i++){
        float scaledSigma = sigma * pow(k, i);

        Mat curGauss = calculateGaussian(source, scaledSigma);

        // square dog image
        Mat dogImage = MatrixHelper::applyOperator(curGauss, prevGauss, "substract");
        Mat squareDogImage = MatrixHelper::applyOperator(dogImage, dogImage, "multiply");
        
        maxLogValues.push_back(MatrixHelper::getMaxValue(squareDogImage));
        dogImages.push_back(squareDogImage);

        prevGauss = curGauss;
    }

    return dogImages;
}

vector<Blob> BlobDetector::getLocalMaximumPoints(vector<Mat> listLogImages, const vector<float> &maxLogValues, float logThres, float startSigma){
    Mat firstImage = listLogImages[0];
    int height = firstImage.rows;
    int width = firstImage.cols;
    int nImages = listLogImages.size();

    vector<Blob> candidates;

    for(int i = 0; i < nImages; i++){
        vector<Mat> neighbors;
        if (i == 0){
            neighbors.push_back(listLogImages[i+1]);
        }
        else if (i == nImages-1){
            neighbors.push_back(listLogImages[i-1]);
        }
        else{
            neighbors.push_back(listLogImages[i+1]);
            neighbors.push_back(listLogImages[i-1]);
        }
        int layerBLob = 0;
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                if (MatrixHelper::isLocalMaximaAmongNeighbors(listLogImages[i], y, x, neighbors) && 
                    MatrixHelper::getValueOfMatrix(listLogImages[i], y, x) > logThres*maxLogValues[i]){
                    
                    float radius = pow(k, i+1)*sqrt(2)*startSigma;
                    candidates.push_back({x, y, (int)radius});
                    layerBLob++;
                }
            }
        }

        cout << "choose " << layerBLob << " from layer " << i << " radius: " << pow(k, i+1)*sqrt(2)*startSigma << endl;
    }
    return candidates;
}

void BlobDetector::visualizeResult(const Mat& source, vector<Blob> blobs){
    Mat copy = source.clone();

    for(int i = 0; i < blobs.size(); i++){
        circle(copy, Point(blobs[i].x, blobs[i].y), blobs[i].radius, Scalar(0, 255, 0), 1);
    }

    cout << "blob count: " << blobs.size() << endl;
    imshow("input", source);
    imshow("blob result", copy);
}

int BlobDetector::getLogFilterSize(float sigma){
    int filterSize = 2*ceil(3*sigma)+1;
    return filterSize;//(int)(sigma+5);
}


Mat BlobDetector::calculateLoG(const Mat& source, float sigma){
    int kernelSize = getLogFilterSize(sigma);
    Mat logKernel = KernelGenerator::createLoGkernel(kernelSize, sigma);
    Mat logImage = OpencvHelper::conv2d(source, logKernel);

    return logImage;
}

Mat BlobDetector::calculateGaussian(const Mat& source, float sigma){
    int kernelSize = getLogFilterSize(sigma);
    Mat gaussImage = OpencvHelper::applyGaussianKernel(source, 1, kernelSize, sigma);
    return gaussImage;
}