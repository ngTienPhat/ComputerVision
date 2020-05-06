#ifndef MY_IMAGE_HPP__
#define MY_IMAGE_HPP__

#include "common.hpp"

class MyImage{
private:
    Mat image;

public:
    MyImage();
    //Image(string imageDir);
    MyImage(string imageDir, int loadType=IMREAD_GRAYSCALE);
    
    void showImage(string windowName="Display window", int windowSize=WINDOW_AUTOSIZE);

};

#endif //MY_IMAGE_HPP__
