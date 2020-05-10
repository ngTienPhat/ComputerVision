#include "image_operator.hpp"

Mat ImageOperator::EdgeDetectLaplacian(const Mat& sourceImage){
    // get Laplacian kernel
    Mat laplaceKernel = KernelGenerator::getLaplaceKernel();
    
    cout << "Source image: " ;
    printMatrixInfo(sourceImage);

    // use laplaceKernel to compute Laplacian gradient of source image
    Mat laplacianImage = ImageOperator::conv2d(sourceImage, laplaceKernel, true, true);
    
    cout << "Laplacian image: " ;
    printMatrixInfo(laplacianImage);
    
    //cout << laplacianImage << endl;

    // get max value of laplacian matrix:
    int maxValue = ImageOperator::getMaxValue(laplacianImage);
    cout << "max value: " << maxValue << endl;

    // define slope threshold
    float slopeThres = maxValue*0.1;
    cout << "slope threshold: " << slopeThres << endl;

    // find zero crossing points in laplacian matrix
    Mat zeroCrossingResultImage = ImageOperator::findZeroCrossingPoints(laplacianImage, slopeThres);
    

    return zeroCrossingResultImage;
}

Mat ImageOperator::conv2d(const Mat& source, const Mat& kernel, bool useFloat, bool acceptNegative){
    int sHeight = source.rows;
    int sWidth= source.cols;

    Mat cloneSource = source.clone();
    if (useFloat == true){
        source.convertTo(cloneSource, CV_32FC1);
    }
    Mat result = cloneSource.clone();

    //Mat sClone = Mat(sHeight, sWidth, CV_32FC1 , Scalar(0.0));

    for(int y = 0; y < sHeight; y++){
        for(int x = 0; x < sWidth; x++){
            int res = applyConvolutionAtPosition(cloneSource, x, y, kernel, useFloat);
            res = res > 255 ? 255 : res;
            if (acceptNegative == false){
                res = res < 0 ? 0 : res; 
            }

            if (useFloat){
                result.at<float>(y, x) = res;   
            }
            else{
                result.at<uchar>(y, x) = res;
            }
            
        }
    }
    return result;
}

Mat ImageOperator::conv2d(const Mat& source, const Mat& kernel, string padding, int stride, bool useFloat){
    int sHeight = source.rows;
    int sWidth= source.cols;
    int sChannel = source.channels();

    Mat sClone = source.clone();

    for(int y = 0; y < sHeight; y++){
        for(int x = 0; x < sWidth; x++){
            int res = applyConvolutionAtPosition(source, x, y, kernel, useFloat);
            res = res > 255 ? 255:res;
            res = res < 0 ? 0 : res;
            sClone.at<uchar>(y, x) = res;
        }
    }
    return sClone;
}

int ImageOperator::applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel, bool useFloat){
    int sWidth = source.cols;
    int sHeight = source.rows;

    int kHeight = kernel.rows;
    int kWidth = kernel.cols;

    int startSourceX = x + kWidth/2;
    int startSourceY = y + kHeight/2;

    int convResult_int = 0;
    float convResult_float = 0;

    for(int ky = 0; ky < kHeight; ++ky){
        int sourceY = startSourceY - ky;

        for(int kx = 0; kx < kWidth; ++kx){
            int sourceX =  startSourceX - kx;

            if (sourceY < 0 || sourceY >= sHeight ||sourceX < 0 ||sourceX >= sWidth)
                continue;

            if (useFloat == false)
                convResult_int += (int)source.at<uchar>(sourceY, sourceX) * kernel.at<int>(ky, kx);
            else
                convResult_float += 1.0*source.at<float>(sourceY, sourceX) * kernel.at<float>(ky, kx);
        }
    }
    return (useFloat==true)? convResult_float:convResult_int;
}


Mat ImageOperator::addMatAbs(const Mat& a, const Mat& b){
    Mat result = a.clone();
    int aHeight = a.rows;
    int aWidth = a.cols;

    for(int y = 0; y < aHeight; y++){
        for(int x = 0; x < aWidth; x++){
            int sum = (int) sqrt(double(a.at<uchar>(y, x) * a.at<uchar>(y, x) + b.at<uchar>(y, x) * b.at<uchar>(y, x)));
            sum = sum > 255 ? 255:sum;
            sum = sum < 0 ? 0 : sum;
            result.at<uchar>(y, x) = sum;
        }
    }
    return result;
}

// ---------------------------------------------------------------------------------------------------
// Laplacian helper functions
int ImageOperator::getMaxValue(const Mat& source){
    int result = -1;
    // TODO: find max value of matrix source
    int sHeight = source.rows;
    int sWidth = source.cols;
    for(int y = 0; y < sHeight; y++){
        for(int x = 0; x < sWidth; x++){
            int value = source.at<float>(y, x);
            //cout << "value: " << value;
            result = result > value ? result : value;
            //cout << " result: " << result << endl;
        }
    }
    return result;
}

Mat ImageOperator::findZeroCrossingPoints(const Mat& source, float slopeThres){
    int sHeight = source.rows;
    int sWidth = source.cols;
    int dx[] = {-1, 0};
    int dy[] = {0, 1};

    Mat result = Mat::zeros(sHeight, sWidth, CV_8UC1);

    for(int y = 0; y < sHeight; y++){
        for(int x = 0; x < sWidth; x++){
            checkNonZeroBetween(source, result, y, x, slopeThres);
            checkZeroBetween(source, result, y, x, slopeThres);
        }
    }

    return result;
}

void ImageOperator::checkNonZeroBetween(const Mat& source, Mat& result, int y, int x, float slopeThres){
    float currentPoint = source.at<float>(y, x);
    
    // check rightward
    if (x <= source.cols-1){
        float rightPoint = source.at<float>(y, x + 1);
        if (checkEdgePointCondition(currentPoint, rightPoint, slopeThres)){
            result.at<uchar>(y, x) = 255;
        }
    }
    
    //check downward
    if (y <= source.rows-1){
        float downPoint = source.at<float>(y + 1, x);
        if (checkEdgePointCondition(currentPoint, downPoint, slopeThres)){
            result.at<uchar>(y, x) = 255;
        }
    }
    
}
void ImageOperator::checkZeroBetween(const Mat& source, Mat& result, int y, int x, float slopeThres){
    int dx[] = {-1, 0};
    int dy[] = {0, 1};
    
    //compute local variance
    for(int k = 0; k < 2; k++){
        int x1 = x + dx[k];
        int y1 = y + dy[k];
        int x2 = x - dx[k];
        int y2 = y - dy[k];

        if (x1 < 0 || x1 >= source.cols || x2 < 0 || x2 >= source.cols ||
            y1 < 0 || y1 >= source.rows || y2 < 0 || y2 >= source.rows){
            continue;
        }

        float neighbor1 = source.at<float>(y1, x1);
        float neighbor2 = source.at<float>(y2, x2);
        
        if (checkEdgePointCondition(neighbor1, neighbor2, slopeThres)){
            result.at<uchar>(y, x) = 255;
        }
    }
}

bool ImageOperator::checkEdgePointCondition(float point1, float point2, float slopeThres){
    int sign1 = point1 < 0 ? -1: 1;
    int sign2 = point2 < 0 ? -1: 1;
    return (sign1 != sign2) && (abs(point1) + abs(point2) > slopeThres);
}
