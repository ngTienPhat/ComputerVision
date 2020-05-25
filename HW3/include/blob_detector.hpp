#pragma once

#include "common.hpp"
#include "kernel_generator.hpp"
#include "matrix_helper.hpp"

struct Blob{
    int x;
    int y;
    int radius;
};

class BlobDetector{
private:
    static const float k;

public:
    static void detectBlob_LoG(const Mat& source);
    static void detectBlob_DoG(const Mat& source);

private:
    static vector<Mat> getScaleLaplacianImages(const Mat& source, vector<float> &maxLogValues, float startSigma=1.0);
    static vector<Mat> getScaleLaplacianImages_DoG(const Mat& source, vector<float> &maxLogValues, float startSigma=1.0);

    static vector<Blob> getLocalMaximumPoints(vector<Mat> listLogImages, const vector<float> &maxLogValues, float logThres = 0.3, float startSigma=1.0);
    
    
    static int getLogFilterSize(float sigma);
    static void visualizeResult(const Mat& source, vector<Blob> blobs);
    static Mat calculateLoG(const Mat& source, float sigma);
    static Mat calculateGaussian(const Mat& source, float sigma);

};


