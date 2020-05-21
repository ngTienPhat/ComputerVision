#pragma once

#include"common.hpp"
#include "opencv_helper.hpp"

namespace MatrixHelper{
    void printMatrixInfo(const Mat &source);
    float getValueOfMatrix(const Mat &source, int y, int x);
    void setValueOfMatrix(Mat &source, int y, int x, float value);

    float getMaxValue(const Mat& source);
    Mat applyOperator(const Mat& a, const Mat& b, string operatorName);

    int getMatrixArea(const Mat &source);

    bool isLocalMaxima(const Mat& source, int y, int x, int height, int width, float* currentValue=NULL, int windowSize=3);
    bool isLocalMaximaAmongNeighbors(const Mat& source, int y, int x, const vector<Mat>& neighbors, int windowSize=3);
}
