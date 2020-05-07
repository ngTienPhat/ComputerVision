/**
*    author:  Khanh-Toan Nguyen, 1712822, 17TN, HCMUS.
*    created: 07.05.2020 13:25:30
**/

#include<iostream>
#include<cmath>
#include"Libraries/Headers/opencv2/imgproc/imgproc.hpp"
#include"Libraries/Headers/opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

int sobel_x[3][3] = { { -1, 0, 1 },{ -2, 0, 2 },{ -1, 0, 1 } };
int sobel_y[3][3] = { { -1, -2, -1 },{ 0, 0, 0 },{ 1, 2, 1 } };

int convolution(const Mat &image, int x, int y, int** filter, int filter_rows, int filter_columns) {
	int sum = 0;
	int start_x = x + filter_rows / 2, start_y = y + filter_columns / 2;

	for (int i = 0; i < filter_rows; i++)
	{
		for (int j = 0; j < filter_columns; j++)
		{
			if (start_x - i < 0 || start_y - j < 0 || start_x - i >= image.rows || start_y - j >= image.cols) continue;
			sum += image.at<uchar>(start_x - i, start_y - j)*filter[i][j];
		}
	}
	return sum;
}
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

int main()
{

	Mat src, dst;
	int gx, gy, sum, gx_new, gy_new, sum_new;

	// Load an image
	src = imread("lena.png", IMREAD_GRAYSCALE);
	namedWindow("initial");
	imshow("initial", src);
	waitKey(0);

	dst = src.clone();
	if (!src.data)
	{
		return -1;
	}

	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
			dst.at<uchar>(y, x) = 0;

	for (int y = 0; y < src.rows; y++) {
		for (int x = 0; x < src.cols; x++) {
			gx = xGradient_Sobel(src, y, x);
			gy = yGradient_Sobel(src, y, x);
			sum = abs(gx) + abs(gy);
			sum = sum > 255 ? 255 : sum;
			sum = sum < 0 ? 0 : sum;
			dst.at<uchar>(y, x) = sum;
		}
	}

	namedWindow("final");
	imshow("final", dst);

	waitKey(0);

	return 0;
}