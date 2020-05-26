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

// B. Localize keypoints
    thresholdingExtrema(candidates, octaves);
    cout << "num keypoints after thresholding: " << candidates.size() << endl;
// C. assign orientation
    assignKeypointsOrientation(candidates, octaves);
    cout << "num keypoints after assigning orientation: " << candidates.size() << endl;

// D. descrip keypoint
    createKeypointDescriptor(candidates, octaves);
    cout << "len descriptor vector: " << candidates[0].descriptors.size() << endl;
}

// --------------------------------------------------------
// -------- EXECUTION HELPER FUNCTIONS --------

// D. Create keypoint descriptor
void Sift::createKeypointDescriptor(vector<Extrema> &keypoints, const vector<Octave> &octaves){
    int nKeypoints = keypoints.size();
    float weightKernelSigma = this->descriptorWindowSize/6;
    for(int i = 0; i < nKeypoints; i++){
        Extrema curKeypoint = keypoints[i];
        Mat weightKernel = KernelGenerator::createGaussianKernel(this->descriptorWindowSize+1, weightKernelSigma);
        Mat curDOGimage = getDOGimageGivenKeypoint(curKeypoint, octaves);
        
        // get region with size of this.descriptorWindowSize around keypoint and split weight kernel to fit it
        Mat patch = getPatchOfDescriptorAndWeightKernel(curKeypoint, curDOGimage, weightKernel);
        generateKeypointDescriptorVector(curKeypoint, patch, weightKernel, widthSubregion, descriptorWindowSize+1);
        
        if (curKeypoint.descriptors.size() == 0){
            cout << "keypoint " << i << " has no description" << endl;
        }
        keypoints[i] = curKeypoint;
    }
}

void Sift::generateKeypointDescriptorVector(Extrema& keypoint, const Mat& patch, const Mat& weight, int widthSubRegion, int regionSize){
    int totalSubRegion = (int)widthSubRegion*widthSubRegion;
    int numSubregionAxis = (int)regionSize/widthSubRegion;

    for(int i = 0; i < numSubregionAxis; i++){
        for(int j = 0; j < numSubregionAxis; j++){
            int top = i*widthSubRegion;
            int left = j*widthSubRegion;

            if (i >= numSubregionAxis/2){
                top += 1;
            }
            if (j >= numSubregionAxis/2){
                left += 1;
            }

            int bottom = min(patch.rows-1, top + widthSubRegion);
            int right = min(patch.cols-1, left + widthSubRegion);

            // cout << "start computing orientation " << endl;
            // cout << "top: " << top << " \nleft: " << left << "\nbottom: "<<  bottom << "\nright: " << right << endl;
            // cout << "patch: "; MatrixHelper::printMatrixInfo(patch);

            OrientationHistogram histogramResult;
            if (top >= bottom || left >= right){
                histogramResult.size = descriptorNumBin;
                histogramResult.histogram.resize(histogramResult.size, 0);
            }
            else{
                histogramResult = generateOrientationHistogram(
                    MatrixHelper::getPatch(patch, top, left, bottom, right), 
                    MatrixHelper::getPatch(weight, top, left, bottom, right)
                );
            }
            
            //cout << "finish computing orientation" << endl;
            keypoint.descriptors.insert(keypoint.descriptors.end(), histogramResult.histogram.begin(), histogramResult.histogram.end());
        }
    }
}

Mat Sift::getPatchOfDescriptorAndWeightKernel(const Extrema &keypoint, const Mat& DOGimage, Mat& weightKernel){
    int top = max(0, keypoint.y - descriptorWindowSize/2);
    int left = max(0, keypoint.x - descriptorWindowSize/2);
    int bottom = min(DOGimage.rows-1, keypoint.y+descriptorWindowSize/2);
    int right = min(DOGimage.cols-1, keypoint.x+descriptorWindowSize/2);

    Mat patch = MatrixHelper::getPatch(DOGimage, top, left, bottom, right);  // patch = DOG[top::bottom, left::right]
    weightKernel = MatrixHelper::getPatch(weightKernel, top, left, bottom, right);

    return patch;
}



// C. Orientation assignment
void Sift::assignKeypointsOrientation(vector<Extrema> &keypoints, const vector<Octave> &octaves){
    vector<Extrema> newKeypoints;
    
    float binWidth = 360/this->orientationNumBin;
    int nCurrentKeypoints = keypoints.size();

    for(int i = 0; i < nCurrentKeypoints; i++){
        OrientationHistogram orientationHistogram;
        orientationHistogram.size = orientationNumBin;
        orientationHistogram.histogram.resize(orientationNumBin, 0);

        Extrema keypoint = keypoints[i];
        int x = keypoint.x;
        int y = keypoint.y;
        int dogIndex = keypoint.octaveDogIndex;
        Mat dogImage = octaves[keypoint.octaveIndex].dogImages[dogIndex];
        int dogWidth = dogImage.cols;
        int dogHeight = dogImage.rows;
        
        float sigma = getSigmaFromSpecificDog(keypoint.octaveIndex, keypoint.octaveDogIndex) * 1.5;
        int weightWindowSize = int(2*ceil(sigma)+1);
        Mat weightKernel = KernelGenerator::createGaussianKernel(2*weightWindowSize+1, sigma);
        
        int cx, cy;
        for(int oy = -weightWindowSize; oy <= weightWindowSize; oy++){
            for(int ox = -weightWindowSize; ox <= weightWindowSize; ox++){
                cx = x + ox;
                cy = y + oy;
                if (cx < 0 || cx >= dogWidth || cy < 0 || cy >= dogHeight)
                    continue;
                
                GradientResult gradientResult = getGradientValueOfDOGpoint(cy, cx, dogImage);
                int bin = quantizeOrientationBinOfKeypoint(gradientResult, this->orientationNumBin);
                orientationHistogram.histogram[bin] += MatrixHelper::getValueOfMatrix(weightKernel, cy+weightWindowSize, cx+weightWindowSize)*gradientResult.magnitude;
        
            }
        }

        int maxBinIdx = getMaxHistogramIndex(orientationHistogram);
        keypoint.orientation = getOrientationByFittingParabola(orientationHistogram, maxBinIdx, binWidth);
        int maxHistValueCount = orientationHistogram.histogram[maxBinIdx];

        newKeypoints.push_back(keypoint);

        for(int i = 0; i < this->orientationNumBin; i++){
            if (i == maxBinIdx)
                continue;       
            if (0.8*maxHistValueCount <= orientationHistogram.hist[i]){
                Extrema newKeypoint = keypoint;
                newKeypoint.orientation = getOrientationByFittingParabola(orientationHistogram, i, binWidth);
                newKeypoints.push_back(newKeypoint);
            }
        }
    }

    keypoints = newKeypoints;
}

// C - helper functions
int Sift::quantizeOrientationBinOfKeypoint(const GradientResult &keypointGradient, int numBin){
    int binWidth = 360/numBin;
    int binIdx = int(floor(keypointGradient.theta))/binWidth;
    return binIdx;
}
GradientResult Sift::getGradientValueOfDOGpoint(int y, int x, const Mat& keypointDOG){
    GradientResult result; 

    float dy = MatrixHelper::getValueOfMatrix(keypointDOG, min(keypointDOG.rows-1, y+1), x) -
                MatrixHelper::getValueOfMatrix(keypointDOG, max(0, y-1), x);
    float dx = MatrixHelper::getValueOfMatrix(keypointDOG, y, min(keypointDOG.cols-1, x+1)) -
                MatrixHelper::getValueOfMatrix(keypointDOG, y, max(0, x-1));
    
    
    result.magnitude = sqrt(pow(dy, 2)+pow(dx, 2));
    result.theta = (atan2(dy, dx)+KernelGenerator::pi)*180/KernelGenerator::pi;

    return result;
}
float Sift::getOrientationByFittingParabola(const OrientationHistogram& orientationHistogram, int maxBinIndex, float binWidth){
    float centerValue = maxBinIndex*binWidth + binWidth/2;

    float rightValue, leftValue;
    if (maxBinIndex == this->orientationNumBin-1){
        rightValue = 360 + binWidth/2;
    }
    else{
        rightValue = (maxBinIndex+1)*binWidth + binWidth/2;
    }
    if (maxBinIndex == 0){
        leftValue = -binWidth/2;
    }
    else{
        leftValue = (maxBinIndex-1)*binWidth + binWidth/2;
    }

    Mat A = (Mat_<float>(3,3) << pow(centerValue, 2), centerValue, 1, 
			                     pow(rightValue, 2), rightValue, 1, 
			                      pow(leftValue, 2), leftValue, 1);
    
    Mat B = (Mat_<float>(3,1) << orientationHistogram.hist[maxBinIndex], 
                                orientationHistogram.hist[(maxBinIndex+1 )% this->orientationNumBin],
                                orientationHistogram.hist[(maxBinIndex-1) % this->orientationNumBin]);
    
    Mat output = MatrixHelper::convertMatExprToMat(
        A.inv()*B
    );
    float a = MatrixHelper::getValueOfMatrix(output, 0, 0);
    float b = MatrixHelper::getValueOfMatrix(output, 0, 1);
    if (a==0){
        a = 1e-6;
    }
    return -b/(2*a);
}

//get max bin index, TODO
int Sift::getMaxHistogramIndex(const OrientationHistogram &histogram){
    int index = 0;
    float maxValue = histogram.histogram[index];
    for(int i = 1; i < this->orientationNumBin; i++){
        if (histogram.histogram[i] > maxValue){
            index = i;
            maxValue = histogram.histogram[i];
        }
    }
    return index;
}







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
    int newDogIndex = (int)(keypoint.octaveDogIndex + sigmaOffset);
    if (newDogIndex < 0){
        newDogIndex = 0;
    }
    else if(newDogIndex > this->numScalesPerOctave){
        newDogIndex = this->numScalesPerOctave-1;
    }
    keypoint.octaveDogIndex = newDogIndex;
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
float Sift::getSigmaFromSpecificDog(int octaveIndex, int dogIndex){
    float sigma = this->sigmaScale[dogIndex];
    return sigma;
}

OrientationHistogram Sift::generateOrientationHistogram(const Mat& DOGimage, const Mat& weightKernel){
    OrientationHistogram histogramResult;
    histogramResult.size = this->descriptorNumBin;
    histogramResult.histogram.resize(histogramResult.size, 0);

    int height = DOGimage.rows;
    int width = DOGimage.cols;

    if (height <= 0 || width <= 0){
        return histogramResult;
    }

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            GradientResult gradResult = getGradientValueOfDOGpoint(y, x, DOGimage);
            int binIdx = quantizeOrientationBinOfKeypoint(gradResult, this->descriptorNumBin);
            
            histogramResult.histogram[binIdx] += MatrixHelper::getValueOfMatrix(weightKernel, y, x)*gradResult.magnitude;
        }
    }

    return histogramResult;
}

Mat Sift::getDOGimageGivenKeypoint(const Extrema& keypoint, const vector<Octave> &octaves){
    return octaves[keypoint.octaveIndex].dogImages[keypoint.octaveDogIndex];
}
