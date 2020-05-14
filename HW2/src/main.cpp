#include "command_handler.hpp"

void test_edge_detection(string imageDir) {
	// remove noise
	MyImage my_image = MyImage(imageDir, IMREAD_GRAYSCALE);
    my_image.showImage("input");
	Mat gaussianBlur3x3 = KernelGenerator::getGaussianBlur3x3();
	Mat gaussianBlur5x5 = KernelGenerator::getGaussianBlur5x5();
	Mat removeNoiseImage = my_image.removeNoise(gaussianBlur5x5);


    //test Laplacian edge detection
    MyImage image1 = MyImage(removeNoiseImage);
    image1.showImage("after blur [float]");

    Mat laplacianResult = image1.applyEdgeDetection("laplacian");
    MyImage laplacian_image = MyImage(laplacianResult);
    laplacian_image.showImage("laplacian result");

	//Mat laplacianResult = image1.applyEdgeDetection("laplacian");

	//MyImage laplacian_image = MyImage(laplacianResult);
	//laplacian_image.showImage("laplacian result");

	//test Canny edge detection
	MyImage image2 = MyImage(removeNoiseImage);

	Mat cannyResult = image2.applyEdgeDetection("canny");
	
    MyImage canny_image = MyImage(cannyResult);
	canny_image.showImage("canny result");
}

int main(int argc, char** argv) {
    //string data_dir = "/Users/tienphat/Documents/HCMUS/Computer_Vision/ComputerVision/data";
	//string image_dir = data_dir+"/lena.jpg";

	CommandHandler executor = CommandHandler(argc, argv);
    //executor.execute();
	executor.testAndSave("../result");

	return 0;
}