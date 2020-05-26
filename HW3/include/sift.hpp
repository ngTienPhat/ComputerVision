#pragma once

#include "sift_helper.hpp"
#include "opencv_helper.hpp"
#include "matrix_helper.hpp"

class Sift{
// attributes
private: 
    // Parameters for D
    int descriptorNumBin = 8;
    int widthSubregion = 4;
    int descriptorWindowSize = 16;

    //Parameters for C
    int orientationNumBin=36; // number of orientation bin used in C.
    int kernelSize=5;
    
    // Parameters for A
    float sigma;
    int numOctave;
    int numScalesPerOctave;
    float k;
    vector<float> sigmaScale; // sigma for each scale in 1 octave;
public:
    Sift(float sigma, int numOctave, int numScalesPerOctave, float k=sqrt(2));

    /*Main function*/    
    void execute(const Mat& source);


// ------------------------------------------------------------------------------------------------
// HELPER FUNCTIONS

private:
// D. Create Local Description 
    void createKeypointDescriptor(vector<Extrema> &keypoints, const vector<Octave> &octaves);

    Mat getPatchOfDescriptorAndWeightKernel(const Extrema &keypoint, const Mat& DOGimage, Mat& weightKernel);
    void generateKeypointDescriptorVector(Extrema& keypoint, const Mat& patch, const Mat& weight, int numSubRegion, int regionSize);


// C. Orientation assignment
    void assignKeypointsOrientation(vector<Extrema> &keypoints, const vector<Octave> &octaves);
    
    int quantizeOrientationBinOfKeypoint(const GradientResult &keypointGradient, int numBin);
    GradientResult getGradientValueOfDOGpoint(int y, int x, const Mat& keypointDOG);
    // fit parabola to get accurate orientation of chosen bin
    float getOrientationByFittingParabola(const OrientationHistogram& orientationHistogram, int maxBinIndex, float binWidth);
    //get max bin index
    int getMaxHistogramIndex(const OrientationHistogram &histogram);
    



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


// ------------------------------------------------------------------------------------------------------------------------
// COMMON HELPER FUNCTIONS
    // DoG helper function
    Mat getGaussKernel(float sigma);
    int getGaussKernelSize(float sigma=1.2);
    float getSigmaFromSpecificDog(int octaveIndex, int dogIndex);

    // helper function: generate orientation histogram
    OrientationHistogram generateOrientationHistogram(const Mat& DOGimage, const Mat& weightKernel);
    
    // get DOG matrix of a specific keypoint
    Mat getDOGimageGivenKeypoint(const Extrema& keypoint, const vector<Octave> &octaves);

    // crop kernel to fit matrix size
    //void fitKernelToMatrixSize(Mat& kernel, const Mat& sourceMatrix);
};


