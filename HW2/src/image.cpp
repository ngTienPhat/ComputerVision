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

// Image::Image(string imageDir){
//     this->image = imread(imageDir, IMREAD_GRAYSCALE);
// }

void MyImage::showImage(string windowName, int windowSize){
    namedWindow(windowName, windowSize);
    Mat printedMatrix;
    this->image.convertTo(printedMatrix, CV_8UC1);
    //imshow(windowName, printedMatrix);
    imshow(windowName, printedMatrix);
    waitKey(0);
}

Mat MyImage::applyConv2d(const Mat& kernel){
    return ImageOperator::conv2d(this->image, kernel, true);
}

Mat MyImage::removeNoise(const Mat& kernel) {
	return ImageOperator::conv2d(this->image, kernel, true, false);
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