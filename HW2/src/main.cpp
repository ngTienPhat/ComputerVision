#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"

void test_edge_detection(string imageDir){
    // remove noise
    MyImage my_image = MyImage(imageDir, IMREAD_GRAYSCALE);    
	Mat gaussianBlur3x3 = KernelGenerator::getGaussianBlur3x3();
	Mat gaussianBlur5x5 = KernelGenerator::getGaussianBlur5x5();
	Mat removeNoiseImage = my_image.removeNoise(gaussianBlur5x5);

    //test Laplacian edge detection
    // MyImage image1 = MyImage(removeNoiseImage);
    // Mat laplacianResult = image1.applyEdgeDetection("laplacian");
    // MyImage laplacian_image = MyImage(laplacianResult);
    // laplacian_image.showImage("laplacian result");


    //test Canny edge detection
    MyImage image2 = MyImage(
        ImageOperator::conv2d(my_image.getData(), gaussianBlur3x3, "same", 1, true)
    );
    Mat cannyResult = image2.applyEdgeDetection("canny");
    printMatrixInfo(cannyResult);
    MyImage canny_image = MyImage(cannyResult);
    canny_image.showImage("canny result");
}

int main(){
    string data_dir = "/Users/tienphat/Documents/HCMUS/Computer_Vision/ComputerVision/data";
    string image_dir = data_dir + "/lena.jpg";
    test_edge_detection(image_dir);

    return 0;
}	