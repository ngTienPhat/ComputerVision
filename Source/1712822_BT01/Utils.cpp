/**
*    author:  Khanh-Toan Nguyen, 1712822, 17TN, HCMUS.
*    created: 07.05.2020 18:35:59
**/

#include"Utils.h"

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