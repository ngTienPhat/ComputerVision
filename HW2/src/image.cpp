#include "image.hpp"

MyImage::MyImage(string imageDir, int loadType){
    this->image = imread(imageDir, loadType);
    cout << "image shape: (" << this->image.rows << "," 
            << this->image.cols << "," << this->image.channels() << ")\n";
}

MyImage::MyImage(const Mat &image) {
	this->image = image.clone();
	// cout << "image shape: (" << this->image.rows << ","
	// 	<< this->image.cols << "," << this->image.channels() << ")\n";
}


void MyImage::showImage(string windowName, int windowSize){
    namedWindow(windowName, windowSize);
    Mat printedMatrix;
    this->image.convertTo(printedMatrix, CV_8UC1);
    imshow(windowName, printedMatrix);
    waitKey(0);
}

void MyImage::showImageFromMatrix(const Mat& imageMat, string windowName, int moveX, int moveY){
    namedWindow(windowName, WINDOW_AUTOSIZE);
    moveWindow(windowName, moveX, moveY);

    Mat printedMatrix;
    imageMat.convertTo(printedMatrix, CV_8UC1);
    imshow(windowName, printedMatrix);
}

Mat MyImage::applyConv2d(const Mat& kernel){
    return ImageOperator::conv2d(this->image, kernel, true);
}

Mat MyImage::removeNoise(const Mat& kernel) {
	return ImageOperator::conv2d(this->image, kernel, false, false);
}

Mat MyImage::getData(){
    return this->image;
}

Mat MyImage::applyEdgeDetection(string method){
    if (method == "laplacian"){
        return ImageOperator::EdgeDetectLaplacian(this->image);
    }
    if (method == "canny"){
        return ImageOperator::EdgeDetectCanny(this->image);
    }
    return this->image;
}