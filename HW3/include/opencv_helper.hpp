#pragma once

#include "common.hpp"
#include "kernel_generator.hpp"


namespace OpencvHelper{
    Mat applyGaussianKernel(const Mat& source, float sigmaScale, int kernelSize=3, float sigma=1.0);
    Mat conv2d(const Mat& source, const Mat& kernel);
    Mat derivative(const Mat& source, string axis="x");
}