#pragma once

#include "common.hpp"
#include "kernel_generator.hpp"
#include "matrix_helper.hpp"
#include "image.hpp"

struct Blob{
    int x;
    int y;
    int radius;
};


class BlobDetector{
public:
    static const string result_dir;

private:
    static const float k;


public:
    static vector<Blob> detectBlob_LoG(const Mat& source, float startSigma=1.0, int nLayers=10);
    static vector<Blob> detectBlob_DoG(const Mat& source, float startSigma=1.0, int nLayers=8);

    static Mat visualizeResult(const Mat& source, vector<Blob> blobs);
private:
// helper function for detectBlob_LoG
    static vector<Mat> getScaleLaplacianImages(const Mat& source, vector<float> &maxLogValues, float startSigma=1.0, int nLayers=10);
    
// helper function for detectBlob_DoG
    static vector<Mat> getScaleLaplacianImages_DoG(const Mat& source, vector<float> &maxLogValues, float startSigma=1.0, int nLayers=8);

    static vector<Blob> getLocalMaximumPoints(vector<Mat> listLogImages, const vector<float> &maxLogValues, float logThres = 0.05, float startSigma=1.0);
    
    
    static int getLogFilterSize(float sigma);
    
    static Mat calculateLoG(const Mat& source, float sigma);
    static Mat calculateGaussian(const Mat& source, float sigma);

};


