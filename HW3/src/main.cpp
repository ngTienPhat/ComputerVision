#include "blob_detector.hpp"
#include "image.hpp"
#include "keypoints_matcher.hpp"

void testSift(){
	string  dataDir = "../data/TestImages";
	string imageDir = dataDir + "/01.jpg";

	Mat coloredImage = imread(imageDir, IMREAD_COLOR);
	MyImage testImage(imageDir);

	Sift siftDetector(1.6, 4, 5);

	siftDetector.execute(coloredImage);
}

void testKeypointMatching(){
	string  dataDir = "../data";
	//string trainDir = dataDir + "/training_images/01_1.jpg";
	//string testDir = dataDir + "/TestImages/01.jpg";
	string trainDir = dataDir + "/training_images/train.jpg";
	string testDir = dataDir + "/TestImages/test.jpg";
	

	KeypointsMatcher myMatcher;
	myMatcher.knnMatchTwoImages(trainDir, testDir);
	//myMatcher.knnMatchTwoImages(testDir, trainDir);

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

	//testSift();
	testKeypointMatching();

	return 0;
}