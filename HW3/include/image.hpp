#ifndef MY_IMAGE_HPP__
#define MY_IMAGE_HPP__

#include "image_operator.hpp"

/*
Image helper class
*/

class MyImage{
private:
    Mat image;

public:
    // Constructor
    MyImage();
    MyImage(string imageDir);
	MyImage(const Mat &image);

    // Display images
    void showImage(string windowName="Display window", int windowSize=WINDOW_AUTOSIZE);

    // Save images
    void saveImage(string saveDir, string imageName);

    /* Apply Conv2D with given kernel on this image
    Input: matrix of kernel to compute convolution on
    Output: matrix of result
    */
    Mat applyConv2d(const Mat& kernel);

    /* Apply Conv2D with given kernel on this image
    Input: matrix of kernel to blur image
    Output: matrix of result image
    */
	Mat removeNoise(const Mat& kernel);

    // return image matrix
    Mat getData();

// ---------------------------------------------------------------------------------
// STATIC METHODS
    
    /*
    Display image matrix
    Input:
        imageMat: image matrix to show
        windowName: name of window to show image
        moveX, moveY: coordinate at which display window will locate
    */
    static void showImageFromMatrix(const Mat& imageMat, string windowName="image", int moveX=0, int moveY=0);

    
    /*
    Save image matrix
    Input:
        imageMat: image matrix to save
        saveDir: path to directory to save this image
        imageName: name of this image
    Goal:
        image will be saved at: <saveDir>/<imageName>.jpg
    */
    static void saveImageFromMatrix(const Mat& imageMat, string saveDir, string imageName);
};

#endif //MY_IMAGE_HPP__
