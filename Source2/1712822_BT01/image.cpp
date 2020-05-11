#include "image.hpp"

MyImage::MyImage(string imageDir, int loadType){
    this->image = imread(imageDir, loadType);
    cout << "image shape: (" << this->image.rows << "," 
            << this->image.cols << "," << this->image.channels() << ")\n";
}

MyImage::MyImage(const Mat &image) {
	this->image = image.clone();
	cout << "image shape: (" << this->image.rows << ","
		<< this->image.cols << "," << this->image.channels() << ")\n";
}

// Image::Image(string imageDir){
//     this->image = imread(imageDir, IMREAD_GRAYSCALE);
// }

void MyImage::showImage(string windowName, int windowSize){
    namedWindow(windowName, windowSize);
    imshow("image", this->image);
    waitKey(0);
}

Mat MyImage::applyConv2d(const Mat& kernel){
    return ImageOperator::conv2d(this->image, kernel);
}

Mat MyImage::removeNoise(const Mat& kernel) {
	return ImageOperator::conv2d(this->image, kernel, "same", 1, true);
}
