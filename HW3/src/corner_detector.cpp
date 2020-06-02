#include "corner_detector.hpp"

Mat CornerDetector::harisCornerDetect(const Mat& source, float Rthreshold, float empiricalConstant){
    int height = source.rows;
    int width = source.cols;
    Mat result = Mat::zeros(height, width, CV_32SC1);

    // 1. blur image
    Mat smoothenSource = OpencvHelper::applyGaussianKernel(MatrixHelper::convertToGrayscale(source), 1);

    // 2. calculate Ix, Iy of source image
    Mat Ix = OpencvHelper::derivative(smoothenSource, "x");
    Mat Iy = OpencvHelper::derivative(smoothenSource, "y");

    Mat Ix2 = MatrixHelper::applyOperator(Ix, Ix, "multiply");
    Mat Iy2 = MatrixHelper::applyOperator(Iy, Iy, "multiply");
    Mat Ixy = MatrixHelper::applyOperator(Ix, Iy, "multiply");

    Ix2 = OpencvHelper::applyGaussianKernel(Ix2, 1);
    Iy2 = OpencvHelper::applyGaussianKernel(Iy2, 1);
    Ixy = OpencvHelper::applyGaussianKernel(Ixy, 1);

    // 3. construct R matrix, R(x, y) = det M{x,y} - k.(trace M{x,y})^2
    Mat R = Mat::zeros(height, width, CV_32FC1);
    double maxR = -INT_MAX;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            double detM = (double)MatrixHelper::getValueOfMatrix(Ix2, y, x)*MatrixHelper::getValueOfMatrix(Iy2, y, x) 
                            - (double)pow(MatrixHelper::getValueOfMatrix(Ixy, y, x), 2);

            double traceM = MatrixHelper::getValueOfMatrix(Ix2, y, x) + MatrixHelper::getValueOfMatrix(Iy2, y, x);
            double curRvalue = detM - empiricalConstant*pow(traceM, 2);

            // MatrixHelper::setValueOfMatrix(
            //     R, y, x, curRvalue
            // );
 
            maxR = max(maxR, curRvalue);
            MatrixHelper::setValueOfMatrix(R, y, x, (float)curRvalue);
        }
    }
    //4. Thresholding R matrix
    int numCorner=0;
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            double curValue = MatrixHelper::getValueOfMatrix(R, y, x);
            
            if (curValue > Rthreshold*maxR && MatrixHelper::isLocalMaxima(R, y, x, height, width)){
                MatrixHelper::setValueOfMatrix(result, y, x, 1);
            }
        }
    }
    
    // visualize result
    showResult(source, result);

    return result;
}

void CornerDetector::showResult(const Mat& source, const Mat& result){
    int height = source.rows;
    int width = source.cols;
    int numCorner=0;

    Mat copy = source.clone();
    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            if (MatrixHelper::getValueOfMatrix(result, y, x) == 1){
                circle(copy, Point(x, y), 3, Scalar(0, 255, 0), 1);
                numCorner++;
            }                
        }
    }

    cout << "corner: " << numCorner << endl;
    //MyImage::showImageFromMatrix(source, "input", 0, 0);  
    //MyImage::showImageFromMatrix(copy, "after detect corner", width, 0);
    imshow("input", source);
    imshow("Harris result", copy);
}
