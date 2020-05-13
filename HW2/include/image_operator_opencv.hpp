#ifndef IMAGE_OPERATOR_OPENCV_HPP__
#define IMAGE_OPERATOR_OPENCV_HPP__

#include "common.hpp"
#include "image.hpp"

class opencvImageOperator {
	// PUBLIC FUNCTIONS
public:
	// Gaussian Blur
	static Mat GaussianBlur_opencv(const Mat& sourceImage, int size_of_kernel_gaussian = 3, float signma = 1.0);

	// Sobel edge detection
	static Mat EdgeDetectSobel_opencv(const Mat& sourceImage, float threshold, int size_of_kernel_gaussian = 3, float signma = 1.0);

	// Prewitt edge detection - OpenCV Library dont serve this function.

	// Laplacian edge detection
	static Mat EdgeDetectLaplacian_opencv(const Mat& sourceImage, int size_of_kernel_gaussian = 3, float signma = 1.0);

	// Canny edge detection
	static Mat EdgeDetectCanny_opencv(const Mat& sourceImage, float low_threshold, float high_threshold, int size_of_kernel_gaussian = 3, float signma = 1.0);

	// CONVOLUTION 2D Version 2

	// PRIVATE FUNCTION:
private:
	static void maximizeEdgePixels(Mat& source, float thres);
	// ---------------------------------------------------------------------------------------------------
};

#endif