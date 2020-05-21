#pragma once 
#include "common.hpp"
#include "opencv_helper.hpp"
#include "matrix_helper.hpp"
#include "image.hpp"

class CornerDetector{

public:
static Mat harisCornerDetect(const Mat& source, float Rthreshold=0.01, float empiricalConstant=0.05);
static void showResult(const Mat& source, const Mat& result);

};