#include "image_operator_opencv.hpp"

Mat opencvImageOperator::GaussianBlur_opencv(const Mat& sourceImage, int size_of_kernel){
	Mat result = sourceImage.clone();
	if (size_of_kernel == 3)
		GaussianBlur(sourceImage, result, Size(3, 3), 0, 0, BORDER_DEFAULT);
	else if (size_of_kernel == 5)
		GaussianBlur(sourceImage, result, Size(5, 5), 0, 0, BORDER_DEFAULT);
	return result;
}

Mat opencvImageOperator::EdgeDetectSobel_opencv(const Mat& sourceImage){
	Mat result = sourceImage.clone(), grad_x, grad_y, abs_grad_x, abs_grad_y, src_gray;
	int ddepth = CV_16S, scale = 1, delta = 0, ksize = 1;

	result = GaussianBlur_opencv(result, 3);
	cvtColor(result, src_gray, COLOR_BGR2GRAY);

	Sobel(src_gray, grad_x, ddepth, 1, 0, ksize, scale, delta, BORDER_DEFAULT);
	Sobel(src_gray, grad_y, ddepth, 0, 1, ksize, scale, delta, BORDER_DEFAULT);
	// converting back to CV_8U
	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);

	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, result);
	return result;
}

// Laplacian edge detection
Mat opencvImageOperator::EdgeDetectLaplacian_opencv(const Mat& sourceImage) {
	Mat src, src_gray, dst, result = sourceImage.clone();
	int kernel_size = 3, scale = 1, delta = 0, ddepth = CV_16S;

	result = GaussianBlur_opencv(result, 3);
	cvtColor(src, src_gray, COLOR_BGR2GRAY); // Convert the image to grayscale
	Laplacian(src_gray, dst, ddepth, kernel_size, scale, delta, BORDER_DEFAULT);
	// converting back to CV_8U
	convertScaleAbs(dst, result);

	return result;
}

// Canny edge detection
Mat opencvImageOperator::EdgeDetectCanny_opencv(const Mat& sourceImage, float low_threshold, float high_threshold) {
	Mat result = sourceImage.clone();

	result = GaussianBlur_opencv(result);

	Canny(result, result, low_threshold, high_threshold);
	return result;
}