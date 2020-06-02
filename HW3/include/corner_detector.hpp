#pragma once 
#include "common.hpp"
#include "opencv_helper.hpp"
#include "matrix_helper.hpp"
#include "image.hpp"
#include "sift_helper.hpp"

class CornerDetector{

public:

/*
Main function to execute Haris Corner detection algorithm:
Input:
- source: input image (Matrix)
- Rthreshold: threshold to eliminate local maxima (percentage of max R)
- empiricalConstant: empirical constant alpha in the orginal formula

R = detM - empiricalConstant*(traceM)^2
choose R > threshold T , T = Rthreshold * max(R)
*/
static Mat harisCornerDetect(const Mat& source, float Rthreshold=0.01, float empiricalConstant=0.05);

static void showResult(const Mat& source, const Mat& result);



};