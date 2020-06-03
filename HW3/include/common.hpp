#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;
using namespace cv;


string getImageNameFromImageDir(string imageDir);

float sumFunction(float a, float b);
float multiplyFunction(float a, float b);
float divideFunction(float a, float b);
float substractFuntion(float a, float b);

template<class T>
void extendVector(vector<T> &a, const vector<T> &b);

