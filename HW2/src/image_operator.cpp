#include "image_operator.hpp"

Mat ImageOperator::EdgeDetectCanny(const Mat& sourceImage){
    MyImage image(sourceImage);

    string pad = "same";
    Mat imageGx = ImageOperator::conv2d(sourceImage, KernelGenerator::getPrewittKernelGx(), pad, 1, false);
    Mat imageGy = ImageOperator::conv2d(sourceImage, KernelGenerator::getPrewittKernelGy(), pad, 1, false);

    Mat magnitude = computeMagnitude(imageGx, imageGy);
    Mat direction = computeDirection(imageGx, imageGy);
    
    NonMaxSuppression(direction, magnitude);

    Mat canny_result = HysteresisThresholding(magnitude, 100, 10);
    return canny_result;
}

Mat ImageOperator::EdgeDetectLaplacian(const Mat& sourceImage){
    // get Laplacian kernel
    Mat laplaceKernel = KernelGenerator::getLaplaceKernel();

    // use laplaceKernel to compute Laplacian gradient of source image
    Mat laplacianImage = ImageOperator::conv2d(sourceImage, laplaceKernel, true, true);
    
    //cout << laplacianImage << endl;

    // get max value of laplacian matrix:
    int maxValue = ImageOperator::getMaxValue(laplacianImage);

    // define slope threshold
    float slopeThres = maxValue*0.2;

    // find zero crossing points in laplacian matrix
    Mat zeroCrossingResultImage = ImageOperator::findZeroCrossingPoints(laplacianImage, slopeThres);

    return zeroCrossingResultImage;
}


// ---------------------------------------------------------------------------------------------------
// Common helper functions
Mat ImageOperator::conv2d(const Mat& source, const Mat& kernel, bool useFloat, bool acceptNegative){
    int sHeight = source.rows;
    int sWidth= source.cols;

    Mat result = source.clone();
    if (useFloat==true){
        source.convertTo(result, CV_32FC1);
    }

    for(int y = 0; y < sHeight; y++){
        for(int x = 0; x < sWidth; x++){
            float res = applyConvolutionAtPosition(source, x, y, kernel, useFloat);
            res = res > 255 ? 255 : res;
            if (acceptNegative == false){
                res = res < 0 ? 0 : res; 
            }
            setValueOfMatrix(result, y, x, res);
            // if (useFloat){
            //     result.at<float>(y, x) = res;   
            // }
            // else{
            //     result.at<uchar>(y, x) = res;
            // }
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

float ImageOperator::applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel, bool useFloat){
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
                convResult_int += getValueOfMatrix(source, sourceY, sourceX) * kernel.at<int>(ky, kx);
            else
                convResult_float += getValueOfMatrix(source, sourceY, sourceX) * kernel.at<float>(ky, kx);
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
// Canny helper functions
Mat ImageOperator::computeMagnitude(const Mat& a, const Mat& b){
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
Mat ImageOperator::computeDirection(const Mat& gx, const Mat &gy) {
    Mat result = gx.clone();
    int aHeight = result.rows;
    int aWidth = result.cols;

    double PI = 3.14159265;

    for (int y = 0; y < aHeight; y++) {
        for (int x = 0; x < aWidth; x++) {
            double angle = atan2((double)gy.at<uchar>(y,x),(double) gx.at<uchar>(y, x)) * 180 / PI;
            if (angle <= 22.5)
                angle = 0;
            else if (angle <= 67.5)
                angle = 45;
            else if (angle <= 112.5)
                angle = 90;
            else if (angle <= 157.5)
                angle = 135;
            else //angle > 157.5
                angle = 0;
            result.at<uchar>(y, x) = angle; //0, 45, 90, 135
        }
    }
    return result;
}
void ImageOperator::NonMaxSuppression(const Mat &direction, Mat &gradient) {
    int aHeight = direction.rows;
    int aWidth = direction.cols;

    for (int y = 0; y < aHeight; ++y){
        for (int x = 0; x < aWidth; ++x) {
            int angle = direction.at<uchar>(y, x);
            uchar current_gradient = gradient.at<uchar>(y, x);
            int compare = 0;

            if (angle == 0) //compare gradient at (y, x) with (y, x + 1) & (y, x - 1)
            { 
                compare += (x + 1 < aWidth) ? 
                                (current_gradient >= gradient.at<uchar>(y, x + 1)) : 1;
                compare += (x - 1 >= 0) ? 
                                (current_gradient >= gradient.at<uchar>(y, x - 1)) : 1;
            }
            else if (angle == 45) //compare gradient at (y, x) with (y - 1, x + 1) & (y + 1, x - 1)
            {
                compare += ((x + 1 < aWidth) && (y - 1 >= 0)) ? 
                                (current_gradient >= gradient.at<uchar>(y - 1, x + 1)) : 1;
                compare += ((x - 1 >= 0) && (y + 1 < aHeight)) ? 
                                (current_gradient >= gradient.at<uchar>(y + 1, x - 1)) : 1;
            }
            else if (angle == 90) //compare gradient at (y, x) with (y - 1, x) & (y + 1, x)
            { 
                compare += (y + 1 < aHeight) ? 
                                (current_gradient >= gradient.at<uchar>(y + 1, x)) : 1;
                compare += (y - 1 >= 0) ? 
                                (current_gradient >= gradient.at<uchar>(y - 1, x)) : 1;
            }
            else if (angle == 135) //compare gradient at (y, x) with (y - 1, x - 1) & (y + 1, x + 1)
            { 
                compare += ((x - 1 >= 0) && (y - 1 >= 0)) ?
                                (current_gradient >= gradient.at<uchar>(y - 1, x - 1)) : 1;
                compare += ((x + 1 < aWidth) && (y + 1 < aHeight)) ?
                                (current_gradient >= gradient.at<uchar>(y + 1, x + 1)) : 1;
            }
            if (compare < 2)
                gradient.at<uchar>(y, x) = 0;
        }
    }
}
void ImageOperator::dfs(Mat &canny_mask, const Mat &gradient, int y, int x, double high_threshold, double low_threshold, vector<vector<bool>> &visited) { //dfs at position (y, x)
    int height = canny_mask.rows, width = canny_mask.cols;

    visited[y][x] = true;
    canny_mask.at<uchar>(y, x) = 255;

    for (int step_y = -1; step_y <= 1; ++step_y) {
        for (int step_x = -1; step_x <= 1; ++step_x) {
            if (y + step_y >= height || y + step_y < 0 || x + step_x >= width || x + step_x < 0)
                continue;
            if (visited[y + step_y][x + step_x] == true)
                continue;
            if (gradient.at<uchar>(y + step_y, x + step_x) >= low_threshold)
            {
                dfs(canny_mask, gradient, y + step_y, x + step_x, high_threshold, low_threshold, visited);
            }
        }
    }
}
Mat ImageOperator::HysteresisThresholding(const Mat &gradient, double high_threshold, double low_threshold) {
    //assert(done the non-max suppression step!)
    Mat canny_mask = gradient.clone();
    int aHeight = canny_mask.rows;
    int aWidth = canny_mask.cols;

    for (int y = 0; y < aHeight; ++y)
        for (int x = 0; x < aWidth; ++x)
            canny_mask.at<uchar>(y, x) = 0;

    for (int y = 0; y < aHeight; ++y) {
        for (int x = 0; x < aWidth; ++x) {
            if (gradient.at<uchar>(y, x) < low_threshold)
                canny_mask.at<uchar>(y, x) = 0;
            else if (gradient.at<uchar>(y, x) > high_threshold)
                canny_mask.at<uchar>(y, x) = 255;
        }
    }

    vector<vector<bool>> visited(aHeight, vector<bool>(aWidth, false));

    for (int y = 0; y < aHeight; ++y) 
        for (int x = 0; x < aWidth; ++x) 
            if (canny_mask.at<uchar>(y, x) == 255 && visited[y][x] == false)
                dfs(canny_mask, gradient, y, x, high_threshold, low_threshold, visited);

    return canny_mask;
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
            result = result > value ? result : value;
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
