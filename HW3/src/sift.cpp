#include "sift.hpp"

using namespace MatrixHelper;

Sift::Sift(float sigma, int numOctave, int numScalesPerOctave, float k){
    this->sigma = sigma;
    this->numOctave = numOctave;
    this->numScalesPerOctave = numScalesPerOctave;
    this->k = pow(2, 1/2);

    for(int i = 0; i < numScalesPerOctave+1; i++){
        this->sigmaScale.push_back(sigma*pow(k, i));
    }
}

vector<Extrema> Sift::extractKeypoints(const string& imageDir){
    Mat sourceMatrix = preprocessInputImage(imageDir);
    return execute(sourceMatrix);
}

vector<Extrema> Sift::execute(const Mat& source){
    clock_t start = clock();
    cout << "input image: "; MatrixHelper::printMatrixInfo(source);
    Mat graySource = MatrixHelper::convertToGrayscale(source);

    //0. blur input image:
    float blurSigma = 1.2;
    Mat blurSource = OpencvHelper::applyGaussianKernel(graySource, 1, getGaussKernelSize(blurSigma), blurSigma);
    //resize(blurSource, blurSource, cv::Size(), 2.0, 2.0, INTER_LINEAR);

    cout << "input matrix: "; MatrixHelper::printMatrixInfo(graySource);

// A. Detect candidate key points;
    //A.1. create Gaussian pyramid (stored in list of Octave):
    vector<Octave> octaves = createGaussianPyramid(blurSource);

    cout << "created " << octaves.size() << " octaves" << endl;

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

    //visualizeKeypoints(candidates, source);

//C. assign orientation
    assignKeypointsOrientation(candidates, octaves);
    cout << "num keypoints after assigning orientation: " << candidates.size() << endl;


// D. descrip keypoint
    createKeypointDescriptor(candidates, octaves);
    cout << "len descriptor vector: " << candidates[0].descriptors.size() << endl;
    cout << "time taken: " << (double)(clock() - start)/CLOCKS_PER_SEC << endl;

    //visualizeKeypoints(candidates, source);
    //writeKeypointsToFile("./result/keypoints.txt", candidates);
    
    return candidates;
}

// --------------------------------------------------------
void Sift::writeKeypointsToFile(const string& filename, const vector<Extrema> &keypoints){
    ofstream outFile;
    outFile.open(filename);
    int descripSize = keypoints[0].descriptors.size();
    for(int i = 0; i < keypoints.size(); i++){
        outFile << i << " " << keypoints[i].octaveIndex 
                << " " << keypoints[i].octaveDogIndex 
                << " " << keypoints[i].y
                << " " << keypoints[i].x
                << " " << keypoints[i].orientation << endl;

        for(int j = 0; j < descripSize; j++){
            outFile << keypoints[i].descriptors[j];
            if (j == descripSize-1){
                outFile << "\n";
            }
            else{
                outFile << " ";
            }
        }
    }
    outFile.close();
}

Mat Sift::preprocessInputImage(const string& imageDir){
    Mat coloredImage = imread(imageDir, IMREAD_COLOR);
    return coloredImage;
}

// --------------------------------------------------------
// -------- EXECUTION HELPER FUNCTIONS --------

// D. Create keypoint descriptor
void Sift::createKeypointDescriptor(vector<Extrema> &keypoints, const vector<Octave> &octaves){
    vector<Extrema> newKps;
    int nKeypoints = keypoints.size();
    float weightKernelSigma = this->descriptorWindowSize/6.0;
    for(int i = 0; i < nKeypoints; i++){
        //cout << "generate descriptor for keypoint " << i << endl;
        
        Extrema curKeypoint = keypoints[i];
        Mat weightKernel = KernelGenerator::createGaussianKernel(this->descriptorWindowSize+1, weightKernelSigma);
        Mat curDOGimage = octaves[curKeypoint.octaveIndex].dogImages[curKeypoint.octaveDogIndex];
        //printKeypointInfo(curKeypoint);
        // get region with size of this.descriptorWindowSize around keypoint and split weight kernel to fit it
        int top = max(0, curKeypoint.y - descriptorWindowSize/2);
        int left = max(0, curKeypoint.x - descriptorWindowSize/2);
        int bottom = min(curDOGimage.rows-1, curKeypoint.y+descriptorWindowSize/2);
        int right = min(curDOGimage.cols-1, curKeypoint.x+descriptorWindowSize/2);
        //Mat patch = MatrixHelper::getPatch(curDOGimage, top, left, bottom, right);

        // init list of all histograms for all subregions (default: 16)
        int nSubregion = pow(descriptorWindowSize/widthSubregion, 2); // default: 4^2 = 16
        vector<OrientationHistogram> subregionHistograms;
        subregionHistograms.resize(nSubregion);
        for(int i = 0; i < subregionHistograms.size(); i++){
            subregionHistograms[i].size = descriptorNumBin; // default: 8
            subregionHistograms[i].histogram.resize(descriptorNumBin, 0.0); 
        }


        // loop over all positions in subregion and calculate their orientation
        for(int y = top; y <= bottom; y++){
            for(int x = left; x <= right; x++){
                if(x-left == 2*widthSubregion || y-top == 2*widthSubregion)
                    continue;

                GradientResult gradientResult = getGradientValueOfDOGpoint(y, x, curDOGimage);
                float weight = getValueOfMatrix(weightKernel, y-top, x-left);
                int curHistogramIndex = getSubregionIndexGivenCoordinate(x-left, y-top);
                int binIdx = quantizeOrientationBinOfKeypoint(gradientResult, descriptorNumBin);

                subregionHistograms[curHistogramIndex].histogram[binIdx] += weight*gradientResult.magnitude;
            }
        }

        for(int i = 0; i < subregionHistograms.size(); i++){
            curKeypoint.descriptors.insert(
                curKeypoint.descriptors.end(), 
                subregionHistograms[i].histogram.begin(),
                subregionHistograms[i].histogram.end()
            );
        }
        normalizeDescriptorVector(curKeypoint.descriptors, "L1");
        
        
        if (curKeypoint.descriptors.size() == 0){
            cout << "keypoint " << i << " has no description" << endl;
        }
        
        newKps.push_back(curKeypoint);
        // cout << "descriptor: ";
        // for(int i = 0; i < 8; i++){
        //     cout << curKeypoint.descriptors[i] << "-";
        // }
        // cout << endl;
    }

    keypoints = newKps;
}

int Sift::getSubregionIndexGivenCoordinate(int x, int y){
    int res;
    if (x >= widthSubregion*2)
        x--;
    if (y >= widthSubregion*2)
        y--;
    
    int orderX = x/widthSubregion;
    int orderY = y/widthSubregion;
    res = orderX*(descriptorWindowSize/widthSubregion) + orderY;
    return res;
}

void Sift::normalizeDescriptorVector(vector<float> &descriptorVector, string type){
    float norm=0.0;
    int vectorSize = descriptorVector.size();
    for(int i = 0; i < vectorSize; i++){
        if (type == "L1"){
            norm += abs(descriptorVector[i]);
        }
        else if(type=="L2"){
            norm += descriptorVector[i]*descriptorVector[i];
        }
    }

    for(int i = 0; i < vectorSize; i++){
        descriptorVector[i] /= norm;
    }
}

// C. Orientation assignment
void Sift::assignKeypointsOrientation(vector<Extrema> &keypoints, const vector<Octave> &octaves){
    vector<Extrema> newKeypoints;
    
    float binWidth = 360/this->orientationNumBin;
    int nCurrentKeypoints = keypoints.size();

    for(int i = 0; i < nCurrentKeypoints; i++){
        //cout << "assign orientation keypoint " << i << endl;
        
        OrientationHistogram orientationHistogram;
        orientationHistogram.size = orientationNumBin;
        orientationHistogram.histogram.resize(orientationNumBin, 0.0);

        Extrema keypoint = keypoints[i];

        //printKeypointInfo(keypoint);

        int x = keypoint.x;
        int y = keypoint.y;
        int dogIndex = keypoint.octaveDogIndex;
        Mat dogImage = octaves[keypoint.octaveIndex].dogImages[dogIndex];
        int dogWidth = dogImage.cols;
        int dogHeight = dogImage.rows;
        
        float sigma = abs(1.5*(keypoint.realDOGindex));

        if (keypoint.realDOGindex == 0)
            sigma=1.0;
        //else{
        //  sigma = dogIndex*1.5;
        // }
        int weightWindowSize = int(2*ceil(sigma)+1);
        Mat weightKernel = KernelGenerator::createGaussianKernel(2*weightWindowSize+1, sigma);
        //cout << weightKernel << endl;
        int cx, cy;
        int maxBinIndex = -1;
        float maxBinValue = INT_MIN;

        //cout << "start quantize orientation\n"; 
        for(int oy = -weightWindowSize; oy <= weightWindowSize; oy++){
            for(int ox = -weightWindowSize; ox <= weightWindowSize; ox++){
                cx = x + ox;
                cy = y + oy;
                if (cx < 0 || cx >= dogWidth || cy < 0 || cy >= dogHeight)
                    continue;
                
                GradientResult gradientResult = getGradientValueOfDOGpoint(cy, cx, dogImage);
                
                int bin = quantizeOrientationBinOfKeypoint(gradientResult, orientationNumBin);
                
                // cout << "gradient magnitude: " << gradientResult.magnitude << endl;
                // cout << "weight for magnitude: " << getValueOfMatrix(weightKernel, oy+weightWindowSize, ox+weightWindowSize) << endl;
                
                orientationHistogram.histogram[bin] += abs(getValueOfMatrix(weightKernel, oy+weightWindowSize, ox+weightWindowSize)*gradientResult.magnitude);

                if (orientationHistogram.histogram[bin] > maxBinValue){
                    maxBinValue = orientationHistogram.histogram[bin];
                    maxBinIndex = bin;
                }
            }
        }

        keypoint.orientation = getOrientationByFittingParabola(orientationHistogram, maxBinIndex, binWidth);
        
        newKeypoints.push_back(keypoint);
        int cnt = 0;

        //cout << "orientation histogram \n";

        for(int i = 0; i < orientationHistogram.size; i++){
            //cout << orientationHistogram.histogram[i] << ", ";
            if (i == maxBinIndex)
                continue;      
                 
            if (0.8*maxBinValue < orientationHistogram.histogram[i]){
                Extrema newKeypoint = keypoints[i];
                newKeypoint.orientation = getOrientationByFittingParabola(orientationHistogram, i, binWidth);
                newKeypoints.push_back(newKeypoint);
                cnt ++;
            }
        }
        //cout << endl;

        // cout << "keypoint " << i 
        //     << " x= " << keypoint.x << " y= " << keypoint.y
        //     << " (octave: "<< keypoint.octaveIndex << ", DOG: " << keypoint.octaveDogIndex << ")"
        //     << " add more " << cnt << " points" << endl;
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

    float dy = getValueOfMatrix(keypointDOG, min(keypointDOG.rows-1, y+1), x) -
                getValueOfMatrix(keypointDOG, max(0, y-1), x);
    float dx = getValueOfMatrix(keypointDOG, y, min(keypointDOG.cols-1, x+1)) -
                getValueOfMatrix(keypointDOG, y, max(0, x-1));
    
    
    result.magnitude = sqrt(dy*dy+dx*dx);
    result.theta = atan2(dy, dx)*180/KernelGenerator::pi;
    if (result.theta < 0) result.theta += 360;

    return result;
}

float Sift::getOrientationByFittingParabola(const OrientationHistogram& orientationHistogram, int maxBinIndex, float binWidth){
    float centerValue = maxBinIndex*binWidth + binWidth/2;
    int nHist = orientationHistogram.size;

    float rightValue, leftValue;
    if (maxBinIndex == nHist-1){
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
    
    Mat B = (Mat_<float>(3,1) << orientationHistogram.histogram[maxBinIndex], 
                                orientationHistogram.histogram[(maxBinIndex+1 )% nHist],
                                orientationHistogram.histogram[(maxBinIndex-1) % nHist]);
    
    Mat output = A.inv()*B;
    
    float a = getValueOfMatrix(output, 0, 0);
    float b = getValueOfMatrix(output, 0, 1);
    if (a==0){
        a = 1e-4;
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

    int aboveDOGindex = keypoint.octaveDogIndex+1;
    int belowDOGindex = keypoint.octaveDogIndex-1;

    Mat aboveDOG;
    Mat belowDOG;
    if (aboveDOGindex >= relativeDOGs.size()){
        aboveDOG = Mat::zeros(curDOG.rows, curDOG.cols, CV_32FC1);
        belowDOG = relativeDOGs[keypoint.octaveDogIndex-1];
    }
    else if(belowDOGindex < 0){
        belowDOG = Mat::zeros(curDOG.rows, curDOG.cols, CV_32FC1);
        aboveDOG = relativeDOGs[keypoint.octaveDogIndex+1];
    }
    else{
        aboveDOG = relativeDOGs[keypoint.octaveDogIndex+1];
        belowDOG = relativeDOGs[keypoint.octaveDogIndex-1];
    }
    int y = keypoint.y;
    int x = keypoint.x;

    float dx = (getValueOfMatrix(curDOG, y, x+1) - getValueOfMatrix(curDOG, y, x-1))/2.0;
    float dy = (getValueOfMatrix(curDOG, y+1, x) - getValueOfMatrix(curDOG, y-1, x))/2.0;
    float ds = (getValueOfMatrix(aboveDOG, y, x) - getValueOfMatrix(belowDOG, y, x))/2.0;

    float dxx = getValueOfMatrix(curDOG, y, x+1) + 
                getValueOfMatrix(curDOG, y, x-1) -
                2*getValueOfMatrix(curDOG, y, x);
    float dyy = getValueOfMatrix(curDOG, y+1, x) + 
                getValueOfMatrix(curDOG, y-1, x) -
                2*getValueOfMatrix(curDOG, y, x);
    
    float dss = getValueOfMatrix(aboveDOG, y, x) +
                getValueOfMatrix(belowDOG, y, x) -
                2*getValueOfMatrix(curDOG, y, x);

    float dxy = ((getValueOfMatrix(curDOG, y+1, x+1) - getValueOfMatrix(curDOG, y+1, x-1)) -
                (getValueOfMatrix(curDOG, y-1, x+1) - getValueOfMatrix(curDOG, y-1, x-1)))/4.0;
    
    float dxs = ((getValueOfMatrix(aboveDOG, y, x+1) - getValueOfMatrix(aboveDOG, y, x-1)) -
                (getValueOfMatrix(belowDOG, y, x+1) - getValueOfMatrix(belowDOG, y, x-1)))/4.0;
    
    float dys = ((getValueOfMatrix(aboveDOG, y+1, x) - getValueOfMatrix(aboveDOG, y-1, x)) -
                (getValueOfMatrix(belowDOG, y+1, x) - getValueOfMatrix(belowDOG, y-1, x)))/4.0;
    
    localizationResult.jacobianMatrix = (Mat_<float>(3, 1) << dx, dy, ds); // J = [dx, dy, ds]
    localizationResult.hessianMatrix = (Mat_<float>(3, 3) << dxx, dxy, dxs,
                                                            dxy, dyy, dys,
                                                            dxs, dys, dss);
    

    localizationResult.offset = (-localizationResult.hessianMatrix.inv())*(localizationResult.jacobianMatrix);
    localizationResult.hessianMatrix = (Mat_<float>(2, 2) << dxx, dxy, 
                                                            dxy, dyy) ;
    
    return localizationResult;
}

void Sift::updateKeypointValue(Extrema& keypoint, const LocalizationResult& localizeInfo){
    keypoint.y += getValueOfMatrix(localizeInfo.offset, 0, 1);
    keypoint.x += getValueOfMatrix(localizeInfo.offset, 0, 0);

    float sigmaOffset = getValueOfMatrix(localizeInfo.offset, 0, 2);
    keypoint.realDOGindex = (keypoint.octaveDogIndex + sigmaOffset);
    int newDogIndex = (int)(keypoint.octaveDogIndex + sigmaOffset);
    if (newDogIndex < 0){
        newDogIndex = 0;
    }
    else if(newDogIndex >= this->numScalesPerOctave){
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
        
        float contrast = getValueOfMatrix(kpDOG, curKeypoint.y, curKeypoint.x) 
                        + 0.5*convertMatExprToMat((localizationResult.jacobianMatrix.t()*localizationResult.offset)).at<float>(0,0) ;
        if (abs(contrast) < thresContrast)
            continue;

        // thresholding edge keypoints
        Mat H= localizationResult.hessianMatrix;
        float traceH= getValueOfMatrix(H, 0, 0) + getValueOfMatrix(H, 1, 1);
        float detH= getValueOfMatrix(H, 0, 0)*getValueOfMatrix(H, 1, 1) - 
                    getValueOfMatrix(H, 0, 1)*getValueOfMatrix(H, 1, 0);
        
        float r = traceH*traceH/detH;
        
        if (r > 1.0*(thresR+1)*(thresR+1)/thresR || detH < 0){
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
    Mat input = source.clone();
    float octaveBaseSigma;

    for(int i = 0; i < this->numOctave; i++){
        //octaveBaseSigma = pow(this->k, i+1);
        Octave octave = createOctaveGaussianPyramid(input, i);
        pyramid.push_back(octave);
        
        // scale down 1/2 source image for next octave
        resize(input, input, Size(), 0.5, 0.5, INTER_NEAREST);
    }

    return pyramid;
}

Octave Sift::createOctaveGaussianPyramid(const Mat& inputMatrix, int octaveIndex){
    Octave octave; 
    //octave.gaussImages.push_back(inputMatrix);
    for(int i = 0; i < this->numScalesPerOctave+1; i++){
        Mat kernel = getGaussKernel(this->sigmaScale[i]);
        //cout << "sigma: " << this->sigmaScale[i]<< " kernel: "; MatrixHelper::printMatrixInfo(kernel);
        
        Mat nextImage = OpencvHelper::conv2d(inputMatrix, kernel);
        
        octave.gaussImages.push_back(nextImage); // default: 4
        
    }
    //cout<<"generate octave " << octaveIndex << " with input image: "; MatrixHelper::printMatrixInfo(inputMatrix);
    return octave;
}

void Sift::createDogPyramidFromGaussPyramid(vector<Octave> &octaves){
    int nOctaves = octaves.size();

    for(int i= 0; i < nOctaves; i++){
        
        for(int j = 1; j < octaves[i].gaussImages.size(); j++){
            
            Mat dogImage = MatrixHelper::applyOperator(octaves[i].gaussImages[j], 
                                                        octaves[i].gaussImages[j-1], 
                                                        "substract");
            
            octaves[i].dogImages.push_back(
                dogImage
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
        if (i == 0){
            neighborDogs.push_back(
                MatrixHelper::applyOperator(progOctave.dogImages[1], progOctave.dogImages[1], "multiply")
            );
        }
        else if(i == nDog-1){
            neighborDogs.push_back(
                MatrixHelper::applyOperator(progOctave.dogImages[nDog-2], progOctave.dogImages[nDog-2], "multiply")
            );
        }
        else{
            Mat squaredUpperDogImage = MatrixHelper::applyOperator(progOctave.dogImages[i+1], 
                                                                    progOctave.dogImages[i+1], "multiply");
            Mat squaredLowerDogImage = MatrixHelper::applyOperator(progOctave.dogImages[i-1], 
                                                                    progOctave.dogImages[i-1], "multiply");
            neighborDogs.push_back(squaredUpperDogImage);
            neighborDogs.push_back(squaredLowerDogImage);
        }
        
        
        Mat squaredCurrentDoG = MatrixHelper::applyOperator(currentDog, currentDog, "multiply");
        float maxValueOfSquaredDOG = MatrixHelper::getMaxValue(squaredCurrentDoG);
        // loop over each position and check if it is extrema or not
        for(int y = descriptorWindowSize/2; y < height-descriptorWindowSize/2; y++){
            for(int x = descriptorWindowSize/2; x < width-descriptorWindowSize/2; x++){
                // if (getValueOfMatrix(squaredCurrentDoG, y, x) < 0.2*maxValueOfSquaredDOG)
                //     continue;

                if (MatrixHelper::isLocalMaximaAmongNeighbors(squaredCurrentDoG, y, x, neighborDogs, 3)){
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
            
            histogramResult.histogram[binIdx] += getValueOfMatrix(weightKernel, y, x)*gradResult.magnitude;
        }
    }

    return histogramResult;
}

Mat Sift::getDOGimageGivenKeypoint(const Extrema& keypoint, const vector<Octave> &octaves){
    return octaves[keypoint.octaveIndex].dogImages[keypoint.octaveDogIndex];
}

void Sift::visualizeKeypoints(const vector<Extrema> &keypoints, const Mat& coloredImage){
    Mat copyImage = coloredImage.clone();
    for(int i = 0; i < keypoints.size(); i++){
        if (keypoints[i].octaveIndex==0){
            int x = keypoints[i].x;
            int y= keypoints[i].y;
            float radius = sqrt(2);
            circle(copyImage, Point(x, y), radius, Scalar(255, 0, 0), 2);
        }   
    }
    imshow("sift keypoints", copyImage);
    waitKey(0);
}


void Sift::printKeypointInfo(const Extrema &point){
    cout << "x: " << point.x << ", ";
    cout << "y: " << point.y << ", ";
    cout << "octave index: " << point.octaveIndex << ", ";
    cout << "DOG index: " << point.octaveDogIndex << ", ";
    cout << "real DOG index: " << point.realDOGindex << ", ";
}


// might be redundant

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
                    getPatch(patch, top, left, bottom, right), 
                    getPatch(weight, top, left, bottom, right)
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

