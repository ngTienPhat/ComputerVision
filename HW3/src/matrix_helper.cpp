#include "matrix_helper.hpp"

using namespace MatrixHelper;

void MatrixHelper::printMatrixInfo(const Mat &source) {
	int typeMatrix = source.type();
	string printOut;

	uchar depth = typeMatrix & CV_MAT_DEPTH_MASK;
	uchar chans = 1 + (typeMatrix >> CV_CN_SHIFT);

	switch (depth) {
	case CV_8U:  printOut = "8U"; break;
	case CV_8S:  printOut = "8S"; break;
	case CV_16U: printOut = "16U"; break;
	case CV_16S: printOut = "16S"; break;
	case CV_32S: printOut = "32S"; break;
	case CV_32F: printOut = "32F"; break;
	case CV_64F: printOut = "64F"; break;
	default:     printOut = "User"; break;
	}

	printOut += "C";
	printOut += (chans + '0');

	cout << printOut << " " << source.rows << "x" << source.cols << endl;
}

float MatrixHelper::getValueOfMatrix(const Mat &source, int y, int x) {
	int typeMatrix = source.type();
	uchar depth = typeMatrix & CV_MAT_DEPTH_MASK;

	switch (depth) {
		//case CV_8U:  return (float)source.at<uchar>(y, x);
	case CV_32F: return source.at<float>(y, x);
	default:     return (float)source.at<uchar>(y, x);
	}
}

void MatrixHelper::setValueOfMatrix(Mat &source, int y, int x, float value) {
	int typeMatrix = source.type();
	uchar depth = typeMatrix & CV_MAT_DEPTH_MASK;

	switch (depth) {
	case CV_32F: source.at<float>(y, x) = value; break;
	default:     source.at<uchar>(y, x) = (uchar)value; break;
	}
}

float MatrixHelper::getMaxValue(const Mat& source){
	float result = INT_MIN;
	int height = source.rows;
    int width = source.cols;

	for(int y = 0; y < height; y++){
		for(int x= 0; x < width; x++){
			result = max(result, getValueOfMatrix(source, y, x));
		}
	}
	return result;
}

Mat MatrixHelper::applyOperator(const Mat& a, const Mat& b, string operatorName){
    int height = a.rows;
    int width = a.cols;
    Mat result = Mat::zeros(height, width, CV_32FC1);

    float (*operatorFunc)(float, float);
    if (operatorName == "sum")
        operatorFunc = &sumFunction;
    else if (operatorName == "multiply")
        operatorFunc = &multiplyFunction;
    else if(operatorName == "divide")
        operatorFunc = &divideFunction;
    else if(operatorName == "substract")
        operatorFunc = &substractFuntion;

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            setValueOfMatrix(result, y, x, operatorFunc(
                getValueOfMatrix(a, y, x), getValueOfMatrix(b, y, x)
            ));
        }
    }

    return result;

}

Mat MatrixHelper::convertToGrayscale(const Mat& source){
	Mat res;
	cvtColor(source, res, COLOR_BGR2GRAY);
	return res;
}

int MatrixHelper::getMatrixArea(const Mat &source){
	return source.rows * source.cols;
}

bool MatrixHelper::isLocalMaxima(const Mat& source, int y, int x, int height, int width, float* currentValue, int windowSize){
	
	if (currentValue == NULL){
		currentValue = new float(MatrixHelper::getValueOfMatrix(source, y, x));
	}
	
	for(int r = -windowSize/2; r <= windowSize/2; r++){
		for(int c = -windowSize/2; c <= windowSize/2; c++){
			if (y + r < 0 || y + r >= height || x + c < 0 || x + c >= width)
				continue;
			
			if (getValueOfMatrix(source, y+r, x+c) > *currentValue)
				return false;
		}
	}
	return true;
}

bool MatrixHelper::isLocalMaximaAmongNeighbors(const Mat& source, int y, int x, const vector<Mat>& neighbors, int windowSize){
	int height = source.rows;
	int width = source.cols;
	float* currentValue = new float(MatrixHelper::getValueOfMatrix(source, y, x));
	for(int i = 0; i < neighbors.size(); i++){
		if (isLocalMaxima(neighbors[i], y, x, height, width, currentValue, windowSize) == false){
			return false;
		}
	}

	return isLocalMaxima(source, y, x, height, width, NULL, windowSize);
}

Mat MatrixHelper::convertMatExprToMat(const MatExpr &matExpr){
	Mat result = Mat(matExpr);
	return result;
}

Mat MatrixHelper::getPatch(const Mat& source, int top, int left, int bottom, int right){
	int patchWidth = right-left+1;
	int patchHeight = bottom-top+1;
	Mat patch(patchHeight, patchWidth, CV_32FC1);

	for(int y = 0; y < patchHeight; y++){
		for(int x = 0; x < patchWidth; x++){
			setValueOfMatrix(patch, y, x, getValueOfMatrix(source, top + y, left + x)); 
		}
	}

	return patch;
}
