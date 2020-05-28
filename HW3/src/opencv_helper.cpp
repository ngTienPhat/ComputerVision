#include "opencv_helper.hpp"

using namespace OpencvHelper;


Mat OpencvHelper::applyGaussianKernel(const Mat& source, float sigmaScale, int kernelSize, float sigma){
    Mat result = source.clone();
    Mat fSource;
    source.convertTo(fSource, CV_32FC1);
    GaussianBlur(fSource, result, Size(kernelSize, kernelSize), sigmaScale*sigma, 0, BORDER_DEFAULT);
    return result;
}

Mat OpencvHelper::derivative(const Mat& source, string axis){
    Mat result;
    Mat kernel;
    if (axis=="x") 
        kernel = KernelGenerator::getSobelKernelGx();
    else 
        kernel = KernelGenerator::getSobelKernelGy();

    filter2D(source, result, CV_32F, kernel);
    return result;
}

Mat OpencvHelper::conv2d(const Mat& source, const Mat& kernel){
    Mat result;
    filter2D(source, result, -1, kernel);
    return result;
}
