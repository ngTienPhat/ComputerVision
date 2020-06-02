#include "blob_detector.hpp"
#include "image.hpp"
#include "keypoints_matcher.hpp"
#include "command_handler.hpp"

void testSift(){
	string  dataDir = "../data/training_images";
	string imageDir = dataDir + "/01_1.jpg";

	Mat coloredImage = imread(imageDir, IMREAD_COLOR);
	MyImage testImage(imageDir);

	Sift siftDetector(1.6, 4, 5);

	siftDetector.execute(coloredImage);
}
void testKeypointMatching(){
	string  dataDir = "../data";
	string trainDir = dataDir + "/TestImages/01.jpg";
	string testDir = dataDir + "/training_images/01_1.jpg";
	
	// trainDir = dataDir + "/training_images/train.jpg";
	// testDir = dataDir + "/TestImages/test.jpg";
	

	KeypointsMatcher myMatcher;
	myMatcher.knnMatchTwoImages(trainDir, testDir);
	//myMatcher.knnMatchTwoImages(testDir, trainDir);

}
void testHaris(){
	string  dataDir = "../data";
	string imageDir = dataDir + "/TestImages/01.jpg";

	Mat coloredImage = imread(imageDir);
	MyImage testImage(imageDir);	
	
	Mat result = CornerDetector::harisCornerDetect(testImage.getData());
	CornerDetector::showResult(coloredImage, result);

	waitKey(0);
}
void testBlob(){
	string  dataDir = "../data";
	string imageDir = dataDir + "/TestImages/01.jpg";

	Mat coloredImage = imread(imageDir);
	MyImage testImage(imageDir);	

	BlobDetector::detectBlob_LoG(coloredImage);
	
	waitKey(0);
}
void testBlobDog(){
	string  dataDir = "../data";
	string imageDir = dataDir + "/sunflower.jpg";

	Mat coloredImage = imread(imageDir);
	MyImage testImage(imageDir);	

	BlobDetector::detectBlob_DoG(coloredImage);
	
	waitKey(0);
}


int main(int argc, char** argv) {
	//testHaris();
	//testBlob();
	//testBlobDog();
	//testSift();
	//testKeypointMatching();

	CommandHandler parser(argc, argv);
	parser.execute();

	return 0;
}