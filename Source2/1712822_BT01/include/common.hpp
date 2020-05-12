#ifndef COMMON_HPP__
#define COMMON_HPP__

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

void printMatrixInfo(const Mat &source);

float getValueOfMatrix(const Mat &source, int y, int x);
void setValueOfMatrix(Mat &source, int y, int x, float value);

#endif //COMMON_HPP__