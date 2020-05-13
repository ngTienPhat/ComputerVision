#ifndef IMAGE_OPERATOR_OPENCV_HPP__
#define IMAGE_OPERATOR_OPENCV_HPP__

#include "common.hpp"
#include "image.hpp"

class opencvImageOperator {
	// PUBLIC FUNCTIONS
public:
	// Gaussian Blur
	static Mat GaussianBlur_opencv(const Mat& sourceImage, int size_of_kernel = 3);

	// Sobel edge detection
	static Mat EdgeDetectSobel_opencv(const Mat& sourceImage);

	// Prewitt edge detection - OpenCV Library dont serve this function.

	// Laplacian edge detection
	static Mat EdgeDetectLaplacian_opencv(const Mat& sourceImage);

	// Canny edge detection
	static Mat EdgeDetectCanny_opencv(const Mat& sourceImage, float low_threshold, float high_threshold);

	// CONVOLUTION 2D Version 2

	// PRIVATE FUNCTION:
private:
	// ---------------------------------------------------------------------------------------------------
};

#endif