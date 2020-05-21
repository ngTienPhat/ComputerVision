#ifndef IMAGE_OPERATOR_HPP__
#define IMAGE_OPERATOR_HPP__

#include "common.hpp"
#include "image.hpp"
#include "matrix_helper.hpp"

class ImageOperator {

public:
	// CONVOLUTION 2D Version 2
	static Mat conv2d(const Mat& source, const Mat& kernel, bool acceptNegative = false, bool acceptExceed = false);

private:
	// ---------------------------------------------------------------------------------------------------
	// Conv2D helper functions
	static float applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel);

};

#endif //IMAGE_OPERATOR_HPP__