/**
*    author:  Khanh-Toan Nguyen, 1712822, 17TN, HCMUS.
*    created: 07.05.2020 18:45:54
**/

#include"sobelFiltering.h"

int sobel_x[3][3] = { { -1, 0, 1 },{ -2, 0, 2 },{ -1, 0, 1 } };
int sobel_y[3][3] = { { -1, -2, -1 },{ 0, 0, 0 },{ 1, 2, 1 } };

int xGradient_Sobel(const Mat &image, int x, int y) {
	int** sobel_filter_x = new int*[3];
	for (int i = 0; i < 3; ++i) {
		sobel_filter_x[i] = new int[3];
		for (int j = 0; j < 3; j++) {
			sobel_filter_x[i][j] = sobel_x[i][j];
		}
	}
	int sum = convolution(image, x, y, sobel_filter_x, 3, 3);

	for (int i = 0; i < 3; ++i)
		delete sobel_filter_x[i];
	if (sobel_filter_x) delete sobel_filter_x;

	return sum;
}
int yGradient_Sobel(const Mat &image, int x, int y) {
	int** sobel_filter_y = new int*[3];
	for (int i = 0; i < 3; ++i) {
		sobel_filter_y[i] = new int[3];
		for (int j = 0; j < 3; j++) {
			sobel_filter_y[i][j] = sobel_y[i][j];
		}
	}
	int sum = convolution(image, x, y, sobel_filter_y, 3, 3);

	for (int i = 0; i < 3; ++i)
		delete sobel_filter_y[i];
	if (sobel_filter_y) delete sobel_filter_y;

	return sum;
}
int detectBySobel(const Mat &src, Mat &dst, Mat &gxMat, Mat &gyMat) {
	if (!src.data) {
		return -1;
	}
	dst = src.clone();
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			dst.at<uchar>(y, x) = 0;

	gxMat = dst.clone();
	gyMat = dst.clone();

	int gx, gy, sum;

	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			gx = xGradient_Sobel(src, y, x);
			gy = yGradient_Sobel(src, y, x);
			sum = (int)sqrt((double)(gx*gx + gy*gy));
			sum = sum > 255 ? 255 : sum;
			sum = sum < 0 ? 0 : sum;
			dst.at<uchar>(y, x) = sum;
			gxMat.at<uchar>(y, x) = gx;
			gyMat.at<uchar>(y, x) = gy;
		}
	}
	return 1;
}