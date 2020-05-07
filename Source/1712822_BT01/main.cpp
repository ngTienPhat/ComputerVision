/**
*    author:  Khanh-Toan Nguyen, 1712822, 17TN, HCMUS.
*    created: 07.05.2020 13:25:30
**/
#include"sobelFiltering.h"

//int prewitt_x[3][3] = { {-1, 0, 1},{-1, 0, 1},{-1, 0, 1} };
//int prewitt_y[3][3] = { {-1, -1, -1}, {0, 0, 0}, {1, 1, 1} };

int main()
{
	Mat src, dst, gxMat, gyMat;

	src = imread("lena.png", IMREAD_GRAYSCALE);
	if (!src.data)
	{
		cout << "Khong mo duoc anh" << endl;
		return -1;
	}
	namedWindow("source image");
	imshow("source image", src);
	waitKey(0);

	int detectEdgeSobel = detectBySobel(src, dst, gxMat, gyMat);

	namedWindow("destination image");
	imshow("destination image", dst);
	waitKey(0);

	return 0;
}