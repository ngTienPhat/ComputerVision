#include "matrix_helper.hpp"
#include "opencv_helper.hpp"
#include "corner_detector.hpp"
#include "blob_detector.hpp"
#include "image.hpp"
#include "sift.hpp"

void testSift(){
	string  dataDir = "../data";
	string imageDir = dataDir + "/sunflower_small.jpg";

	Mat coloredImage = imread(imageDir);
	MyImage testImage(imageDir);

	Sift siftDetector(1.4, 4, 3);
	siftDetector.execute(testImage.getData());
}
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
void testBlobDog(){
	string  dataDir = "../data";
	string imageDir = dataDir + "/sunflower_small.jpg";

	Mat coloredImage = imread(imageDir);
	MyImage testImage(imageDir);	

	BlobDetector::detectBlob_DoG(coloredImage);
	
	waitKey(0);
}


int main(int argc, char** argv) {

	testSift();

	// Mat a = (Mat_<float>(1,3) << 0, -1, 0); 
	// Mat b = (Mat_<float>(3,1) << 0, -1, 0); 

	// Mat result = MatrixHelper::convertMatExprToMat(a*b);
	// cout << result;
	return 0;
}