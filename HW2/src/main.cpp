#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"

int main(){
    string data_dir = "/Users/tienphat/Documents/HCMUS/Computer_Vision/ComputerVision/data";
    string image_dir = data_dir + "/lena.jpg";

    MyImage my_image = MyImage(image_dir, IMREAD_GRAYSCALE);    

	Mat gaussianBlur3x3 = KernelGenerator::getGaussianBlur3x3();
	Mat gaussianBlur5x5 = KernelGenerator::getGaussianBlur5x5();

	Mat removeNoiseImage = my_image.removeNoise(gaussianBlur5x5);

	my_image = MyImage(removeNoiseImage);

    Mat laplacianResult = my_image.applyEdgeDetection("laplacian");
    MyImage laplacian_image = MyImage(laplacianResult);
    laplacian_image.showImage();

    return 0;
}