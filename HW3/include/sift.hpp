#pragma once

#include "sift_helper.hpp"
#include "opencv_helper.hpp"
#include "matrix_helper.hpp"

class Sift{
// attributes
private: 
    int kernelSize=5;


    float sigma;
    int numOctave;
    int numScalesPerOctave;
    float k;
    vector<float> sigmaScale; // sigma for each scale in 1 octave;
public:
    Sift(float sigma, int numOctave, int numScalesPerOctave, float k=sqrt(2));

    /*Main function*/    
    void execute(const Mat& source);

// helper functions:
private:
//C. Orientation assignment

//B. Localize keypoints
    // B.1 Compute subpixel location of each keypoint
    LocalizationResult computeExtremaOffset(const Extrema &keypoint, const vector<Octave> &octaves);
    void updateKeypointValue(Extrema& keypoint, const LocalizationResult& localizeInfo);

    // B.2 Remove edge or low contrast keypoints
    void thresholdingExtrema(vector<Extrema> &keypoints, const vector<Octave> &octaves, float thresContrast=0.3, float thresR=10);
    
//A. Detect candidate keypoints
    //A.1
    vector<Octave> createGaussianPyramid(const Mat& source);
    
    // A.2
    void createDogPyramidFromGaussPyramid(vector<Octave> &octaves);
    Octave createOctaveGaussianPyramid(const Mat& inputMatrix, float sigma, int octaveIndex);
    
    // A.3
    vector<Extrema> detectExtrema(const vector<Octave>& octaves);
    vector<Extrema> detectExtremaFromOctave(const Octave& progOctave, int octaveIndex);

    // DoG helper function
    Mat getGaussKernel(float sigma);
    int getGaussKernelSize(float sigma=1.2);
    float getSigmaFromSpecificDog(int octaveIndex, int dogIndex);
};


