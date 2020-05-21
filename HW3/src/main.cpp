#include "matrix_helper.hpp"
#include "opencv_helper.hpp"
#include "corner_detector.hpp"
#include "blob_detector.hpp"
#include "image.hpp"

void testHaris(){
	string  dataDir = "../data";
	string imageDir = dataDir + "/lena.png";

	Mat coloredImage = imread(imageDir);
	MyImage testImage(imageDir);	
	
	Mat result = CornerDetector::harisCornerDetect(testImage.getData());
	CornerDetector::showResult(coloredImage, result);

	waitKey(0);
}

void testBlob(){
	string  dataDir = "../data";
	string imageDir = dataDir + "/sunflower_small.jpg";

	Mat coloredImage = imread(imageDir);
	MyImage testImage(imageDir);	

	BlobDetector::detectBlob_LoG(coloredImage);
	
	waitKey(0);
}

void testEigenValues(){
	Mat testMat = (Mat_<float>(3, 3) << 3, 2, 4, 
									 2, 0, 2, 
									 4, 2, 3);
	Mat eValues, eVectors;
	eigen(testMat, eValues, eVectors);
	
	cout << eValues << endl;
	cout << eVectors << endl;
}

int main(int argc, char** argv) {
	//testEigenValues();
	//testHaris();
	testBlob();
	// float k = sqrt(2);
	// int i = 8;
	// cout << pow(k, i)*sqrt(2)*1.4 << endl;
	return 0;
}