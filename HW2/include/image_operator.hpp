#ifndef IMAGE_OPERATOR_HPP__
#define IMAGE_OPERATOR_HPP__

#include "common.hpp"
#include "image.hpp"
#include "kernel_generator.hpp"

class ImageOperator{


// PUBLIC FUNCTIONS
public:

    // Sobel edge detection

    
    // Prewitt edge detection 
    
    
    // Laplacian edge detection
    static Mat EdgeDetectLaplacian(const Mat& sourceImage);

    // Canny edge detection
    static Mat EdgeDetectCanny(const Mat& sourceIamge);

    // CONVOLUTION 2D Version 2
    static Mat conv2d(const Mat& source, const Mat& kernel, bool useFloat=false, bool acceptNegative=false);

    // CONVOLUTION 2D Version 1
    static Mat conv2d(const Mat& source, const Mat& kernel, string padding="same", int stride=1, bool useFloat=false);
    static Mat addMatAbs(const Mat& a, const Mat& b);

// PRIVATE FUNCTION:
private:
    // ---------------------------------------------------------------------------------------------------
    // Conv2D helper functions
    static int applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel, bool useFloat=false);

    // ---------------------------------------------------------------------------------------------------
    // Laplacian helper functions
    static int getLaplacianThreshold();
    static int getMaxValue(const Mat& source);
    
    static Mat findZeroCrossingPoints(const Mat& source, float slopeThres);
    static void checkNonZeroBetween(const Mat& source, Mat& result, int y, int x, float slopeThres);
    static void checkZeroBetween(const Mat& source, Mat& result, int y, int x, float slopeThres);
    static bool checkEdgePointCondition(float point1, float point2, float slopeThres);

    // ---------------------------------------------------------------------------------------------------
    // Canny helper functions
    static Mat computeMagnitude(const Mat& a, const Mat& b);
	static Mat computeDirection(const Mat& gx, const Mat &gy);
	static void NonMaxSuppression(const Mat &direction, Mat &gradient);
	static void dfs(Mat &canny_mask, const Mat &gradient, int y, int x, double high_threshold, double low_threshold, vector<vector<bool>> &visited) ;
	static Mat HysteresisThresholding(const Mat &gradient, double high_threshold, double low_threshold);

};

#endif //IMAGE_OPERATOR_HPP__