/**
*    author:  Khanh-Toan Nguyen, 1712822, 17TN, HCMUS.
*    created: 07.05.2020 18:35:45
**/

#ifndef UTILS_H
#define UTILS_H

#include<iostream>
#include<cmath>
#include"Libraries/Headers/opencv2/imgproc/imgproc.hpp"
#include"Libraries/Headers/opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

int convolution(const Mat &image, int x, int y, int** filter, int filter_rows, int filter_columns);

#endif