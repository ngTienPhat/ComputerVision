#pragma once 
#include "common.hpp"


/*
Octave data type: 
*/
struct Octave{
    int scale; // octave_image_size = raw_image_size/scale
    //vector<Mat> gaussizeImage;
    vector<Mat> gaussImages;
    vector<Mat> dogImages;
};

struct Extrema{
    int x; int y; // coordinate of this extrema point in DoG image
    int octaveIndex; // index of its octave in all generated octaves
    int octaveDogIndex; // index of correspondent DoG layer in that its octave
};

struct LocalizationResult{
    Mat offset; // size [1, 3]
    Mat jacobianMatrix; // size: [1, 3]
    Mat hessianMatrix; // size: [2, 2]
};

struct GradientHistogram{
    int bin[36]={10};
};