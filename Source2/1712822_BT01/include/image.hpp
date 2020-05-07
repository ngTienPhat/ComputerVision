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

    static void showImage(const Mat& imageMat, string imageName="image"){
        imshow(imageName, imageMat);
        waitKey(0);
    }
    void showImage(string windowName="Display window", int windowSize=WINDOW_AUTOSIZE);
    Mat applyConv2d(const Mat& kernel);
	Mat removeNoise(const Mat& kernel);
};

#endif //MY_IMAGE_HPP__
