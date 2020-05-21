#include "image_operator.hpp"


Mat ImageOperator::conv2d(const Mat& source, const Mat& kernel, bool acceptNegative, bool acceptExceed) {
	/*
	Input: SourceImage, Kernel, acceptNegative (true if value of pixels can be negative), acceptExceed (true if value of pixels can exceed 255)
	Key task: do the convolution operator of SourceImage with kernel provided.
	Output: conv2d result Matrix
	*/
	int sHeight = source.rows;
	int sWidth = source.cols;

	Mat result = Mat::zeros(sHeight, sWidth, CV_32FC1);

	for (int y = 0; y < sHeight; y++) {
		for (int x = 0; x < sWidth; x++) {
			float res = applyConvolutionAtPosition(source, x, y, kernel);
			if (acceptExceed == false) 
				res = res > 255 ? 255 : res;
			if (acceptNegative == false) 
				res = res < 0 ? 0 : res;
			MatrixHelper::setValueOfMatrix(result, y, x, res);
		}
	}
	return result;
}

float ImageOperator::applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel) {
	/*
	Input: SourceImage, Kernel, position_x_axis, position_y_axis
	Key task: do the convolution operator of pixel[y][x] of SourceImage.
	Output: conv2d result of the corresponding pixel.
	*/
	int sWidth = source.cols;
	int sHeight = source.rows;

	int kHeight = kernel.rows;
	int kWidth = kernel.cols;

	int startSourceX = x + kWidth / 2;
	int startSourceY = y + kHeight / 2;

	float convResult = 0;

	for (int ky = 0; ky < kHeight; ++ky) {
		int sourceY = startSourceY - ky;

		for (int kx = 0; kx < kWidth; ++kx) {
			int sourceX = startSourceX - kx;

			if (sourceY < 0 || sourceY >= sHeight || sourceX < 0 || sourceX >= sWidth)
				continue;

			convResult += MatrixHelper::getValueOfMatrix(source, sourceY, sourceX) * MatrixHelper::getValueOfMatrix(kernel, ky, kx);
		}
	}
	return convResult;
}
