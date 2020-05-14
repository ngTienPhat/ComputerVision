#include "common.hpp"

void printMatrixInfo(const Mat &source) {
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

float getValueOfMatrix(const Mat &source, int y, int x) {
	int typeMatrix = source.type();
	uchar depth = typeMatrix & CV_MAT_DEPTH_MASK;

	switch (depth) {
		//case CV_8U:  return (float)source.at<uchar>(y, x);
	case CV_32F: return source.at<float>(y, x);
	default:     return (float)source.at<uchar>(y, x);
	}
}

void setValueOfMatrix(Mat &source, int y, int x, float value) {
	int typeMatrix = source.type();
	uchar depth = typeMatrix & CV_MAT_DEPTH_MASK;

	switch (depth) {
	case CV_32F: source.at<float>(y, x) = value; break;
	default:     source.at<uchar>(y, x) = (uchar)value; break;
	}
}

Mat addMatrix(const Mat& a, const Mat& b){
    int height = a.rows;
    int width = a.cols;

    Mat result = a.clone();
    for (int y = 0; y < height; y++){
        for(int x = 0; x < width; x++){
            int sum = (int)getValueOfMatrix(a, y, x) + (int)getValueOfMatrix(b, y, x);
            sum = sum > 255 ? 255 : sum;
            setValueOfMatrix(result, y, x, sum);
        }
    }   
    return result;
}

string getImageNameFromImageDir(string imageDir){
	int sLength = imageDir.size();
	int start = 0;
	string imageName= "";

	for(int j = sLength-1; j >= 0; --j){
		if (imageDir[j] == '/'){
			start = j+1;
			break;
		}
	}
	while(imageDir[start] != '.'){
		imageName += imageDir[start];
		start+=1;
	}

	return imageName;
}