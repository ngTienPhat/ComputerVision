#ifndef MY_IMAGE_HPP__
#define MY_IMAGE_HPP__

#include "image_operator.hpp"

class MyImage{
private:
    Mat image;

public:
    MyImage();
    //Image(string imageDir);
    MyImage(string imageDir, int loadType=IMREAD_GRAYSCALE);
	MyImage(const Mat &image);


    void showImage(string windowName="Display window", int windowSize=WINDOW_AUTOSIZE);
    
    Mat applyConv2d(const Mat& kernel);
	Mat removeNoise(const Mat& kernel);
    Mat getData();
// Detect edge
    Mat applyEdgeDetection(string method);

// STATIC METHODS
    static void showImageFromMatrix(const Mat& imageMat, string windowName="image", int moveX=0, int moveY=0);
};

#endif //MY_IMAGE_HPP__
