#include "sift.hpp"
Sift::Sift(float sigma, int numOctave, int numScalesPerOctave, float k){
    this->sigma = sigma;
    this->numScalesPerOctave = numScalesPerOctave;
    this->numOctave = numOctave;
    this->k = pow(k, 1/numScalesPerOctave);

    for(int i = 0; i < numScalesPerOctave+2; i++){
        this->sigmaScale.push_back(sigma*pow(k, i+1));
    }
}

void Sift::execute(const Mat& source){
    //0. blur input image:
    Mat blurSource = OpencvHelper::applyGaussianKernel(source, 1, kernelSize, this->sigma);

// A. Detect candidate key points;
    //A.1. create Gaussian pyramid (stored in list of Octave):
    vector<Octave> octaves = createGaussianPyramid(blurSource);
    //A.2. create DoG pyramid also stored in list of Octave above
    createDogPyramidFromGaussPyramid(octaves);

    cout << "num gaussImage per octave: " << octaves[0].gaussImages.size() << endl;
    cout << "num DoG per octave: " << octaves[0].dogImages.size() << endl;

    //A.3. Extrema detection, values are stored in list of Extrema 
    vector<Extrema> candidates = detectExtrema(octaves);
    cout << "Extrema: " << candidates.size() << endl;
    // test module A:

// B. 
    thresholdingExtrema(candidates, octaves);

}

// --------------------------------------------------------
// -------- EXECUTION HELPER FUNCTIONS --------

// B. 
// B.1 Compute subpixel location of each keypoint
LocalizationResult Sift::computeExtremaOffset(const Extrema &keypoint, const vector<Octave> &octaves){
    LocalizationResult localizationResult;
    
    vector<Mat> relativeDOGs = octaves[keypoint.octaveIndex].dogImages;
    Mat curDOG = relativeDOGs[keypoint.octaveDogIndex];
    Mat aboveDOG = relativeDOGs[keypoint.octaveDogIndex+1];
    Mat belowDOG = relativeDOGs[keypoint.octaveDogIndex-1];

    int y = keypoint.y;
    int x = keypoint.x;

    float dx = (MatrixHelper::getValueOfMatrix(curDOG, y, x+1) - MatrixHelper::getValueOfMatrix(curDOG, y, x-1))/2;
    float dy = (MatrixHelper::getValueOfMatrix(curDOG, y+1, x) - MatrixHelper::getValueOfMatrix(curDOG, y-1, x))/2;
    float ds = (MatrixHelper::getValueOfMatrix(aboveDOG, y, x) - MatrixHelper::getValueOfMatrix(belowDOG, y, x))/2;

    float dxx = MatrixHelper::getValueOfMatrix(curDOG, y, x+1) + 
                MatrixHelper::getValueOfMatrix(curDOG, y, x-1) -
                2*MatrixHelper::getValueOfMatrix(curDOG, y, x);
    float dyy = MatrixHelper::getValueOfMatrix(curDOG, y+1, x) + 
                MatrixHelper::getValueOfMatrix(curDOG, y-1, x) -
                2*MatrixHelper::getValueOfMatrix(curDOG, y, x);
    
    float dss = MatrixHelper::getValueOfMatrix(aboveDOG, y, x) +
                MatrixHelper::getValueOfMatrix(belowDOG, y, x) -
                2*MatrixHelper::getValueOfMatrix(curDOG, y, x);

    float dxy = ((MatrixHelper::getValueOfMatrix(curDOG, y+1, x+1) - MatrixHelper::getValueOfMatrix(curDOG, y+1, x-1)) -
                (MatrixHelper::getValueOfMatrix(curDOG, y-1, x+1) - MatrixHelper::getValueOfMatrix(curDOG, y-1, x-1)))/4;
    
    float dxs = ((MatrixHelper::getValueOfMatrix(aboveDOG, y, x+1) - MatrixHelper::getValueOfMatrix(aboveDOG, y, x-1)) -
                (MatrixHelper::getValueOfMatrix(belowDOG, y, x+1) - MatrixHelper::getValueOfMatrix(belowDOG, y, x-1)))/4;
    
    float dys = ((MatrixHelper::getValueOfMatrix(aboveDOG, y+1, x) - MatrixHelper::getValueOfMatrix(aboveDOG, y-1, x)) -
                (MatrixHelper::getValueOfMatrix(belowDOG, y+1, x) - MatrixHelper::getValueOfMatrix(belowDOG, y-1, x)))/4;
    
    localizationResult.jacobianMatrix = (Mat_<float>(3, 1) << dx, dy, ds); // J = [dx, dy, ds]
    localizationResult.hessianMatrix = (Mat_<float>(3, 3) << dxx, dxy, dxs,
                                                            dxy, dyy, dys,
                                                            dxs, dys, dss);
    

    localizationResult.offset = MatrixHelper::convertMatExprToMat((-localizationResult.hessianMatrix.inv())*(localizationResult.jacobianMatrix));
    localizationResult.hessianMatrix = (Mat_<float>(2, 2) << dxx, dxy, 
                                                            dxy, dyy) ;
    
    return localizationResult;

}
void Sift::updateKeypointValue(Extrema& keypoint, const LocalizationResult& localizeInfo){
    keypoint.y += MatrixHelper::getValueOfMatrix(localizeInfo.offset, 0, 1);
    keypoint.x += MatrixHelper::getValueOfMatrix(localizeInfo.offset, 0, 0);

    float sigmaOffset = MatrixHelper::getValueOfMatrix(localizeInfo.offset, 0, 2);
    keypoint.octaveDogIndex = (int)(keypoint.octaveDogIndex + sigmaOffset);
}

// B.2 Remove keypoints with low contrast 
void Sift::thresholdingExtrema(vector<Extrema> &keypoints, const vector<Octave> &octaves, float thresContrast, float thresR){
    int nKeypoints = keypoints.size();
    Mat kpDOG;
    Extrema curKeypoint;
    vector<Extrema> finalKeypoints;

    for(int i = 0; i < nKeypoints; i++){
        curKeypoint = keypoints[i];
        LocalizationResult localizationResult = computeExtremaOffset(curKeypoint, octaves);
        kpDOG = octaves[curKeypoint.octaveIndex].dogImages[curKeypoint.octaveDogIndex];
        
        // thresholding low-contrast keypoints
        
        float contrast = MatrixHelper::getValueOfMatrix(kpDOG, curKeypoint.y, curKeypoint.x) + 
                            0.5*MatrixHelper::convertMatExprToMat(localizationResult.jacobianMatrix.t() * localizationResult.offset).at<float>(0);
        if (abs(contrast) < thresContrast)
            continue;

        // thresholding edge keypoints
        Mat eigenValues;
        eigen(localizationResult.hessianMatrix, eigenValues);
        float a = MatrixHelper::getValueOfMatrix(eigenValues, 0, 0);
        float b = MatrixHelper::getValueOfMatrix(eigenValues, 0, 1);
        float r = a/b;
        if (pow(r+1, 2)/r > thresR){
            continue;
        }

        updateKeypointValue(curKeypoint, localizationResult);
        finalKeypoints.push_back(curKeypoint);
    }

    keypoints = finalKeypoints;
    cout << "num keypoints after thresholding: " << finalKeypoints.size() << endl;
}

// A. 
vector<Octave> Sift::createGaussianPyramid(const Mat& source){
    // "source" has already been blured with sigma = this->sigma
    vector<Octave> pyramid;
    Mat input = source;
    float octaveBaseSigma;

    for(int i = 0; i < this->numOctave; i++){
        octaveBaseSigma = pow(this->k, i+1);
        Octave octave = createOctaveGaussianPyramid(input, octaveBaseSigma, i);
        pyramid.push_back(octave);
        
        resize(octave.gaussImages[octave.gaussImages.size()-3], input, Size(), 0.5, 0.5);
    }

    return pyramid;
}

Octave Sift::createOctaveGaussianPyramid(const Mat& inputMatrix, float sigma, int octaveIndex){
    Octave octave; 
    octave.gaussImages.push_back(inputMatrix);

    int startK= 1;
    if (octaveIndex > 0){
        startK = this->k;
    }

    for(int i = 0; i < this->numScalesPerOctave+2; i++){
        Mat kernel = getGaussKernel(this->sigmaScale[i]);
        Mat nextImage = OpencvHelper::conv2d(inputMatrix, kernel);
        
        octave.gaussImages.push_back(nextImage);

        //cout << "octave" << octaveIndex << " layer" << i << "gaussImage: " << nextImage << endl;
    }

    return octave;
}

void Sift::createDogPyramidFromGaussPyramid(vector<Octave> &octaves){
    int nOctaves = octaves.size();
    for(int i= 0; i < nOctaves; i++){
        for(int j = 1; j < octaves[i].gaussImages.size(); j++){
            Mat dogImage = MatrixHelper::applyOperator(octaves[i].gaussImages[j], octaves[i].gaussImages[j-1], "substract");
            octaves[i].dogImages.push_back(
                MatrixHelper::applyOperator(dogImage, dogImage, "multiply")
            );
        }
    }
}


// --------------------------------------------------------
// -------- EXTREMA DETECTION HELPER FUNCTIONS --------
vector<Extrema> Sift::detectExtrema(const vector<Octave>& octaves){
    vector<Extrema> result;
    int nOctaves = octaves.size();
    for(int i = 0; i < nOctaves; i++){
        vector<Extrema> octaveExtremas = detectExtremaFromOctave(octaves[i], i);
        //extendVector(result, octaveExtremas);

        result.reserve(result.size()+distance(octaveExtremas.begin(), octaveExtremas.end()));
        result.insert(result.end(), octaveExtremas.begin(), octaveExtremas.end());

        cout << "detect " << octaveExtremas.size() << " from octave " << i << endl;
    }

    return result;
}


vector<Extrema> Sift::detectExtremaFromOctave(const Octave& progOctave, int octaveIndex){
    vector<Extrema> octaveExtremas;
    int nDog = progOctave.dogImages.size();
    // loop over each DoG image
    for(int i = 1; i < nDog-1; i++){
        Mat currentDog = progOctave.dogImages[i];
        int height = currentDog.rows;
        int width = currentDog.cols;

        vector<Mat> neighborDogs;
        neighborDogs.push_back(progOctave.dogImages[i-1]);
        neighborDogs.push_back(progOctave.dogImages[i+1]);


        // loop over each position and check if it is extrema or not
        for(int y = 0; y < height; y++){
            for(int x= 0; x < width; x++){
                if (MatrixHelper::isLocalMaximaAmongNeighbors(currentDog, y, x, neighborDogs, 3)){
                    Extrema extrema({x, y, octaveIndex, i});
                    octaveExtremas.push_back(extrema);
                }
            }
        }
    }

    return octaveExtremas;
}


// --------------------------------------------------------
// -------- GAUSSIAN HELPER FUNCTIONS --------
Mat Sift::getGaussKernel(float sigma){
    int kernelSize = getGaussKernelSize(sigma);
    return KernelGenerator::createGaussianKernel(kernelSize, sigma);
}
int Sift::getGaussKernelSize(float sigma){
    int filterSize = 2*ceil(3*sigma)+1;
    return filterSize;//(int)(sigma+5);
}

