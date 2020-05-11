#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"

int main(){
    string image_dir = "lena.png";

    MyImage my_image = MyImage(image_dir, IMREAD_GRAYSCALE);    

	Mat gaussianBlur3x3 = KernelGenerator::getGaussianBlur3x3();
	Mat gaussianBlur5x5 = KernelGenerator::getGaussianBlur5x5();
    Mat sobelGx = KernelGenerator::getSobelKernelGx();
    Mat sobelGy = KernelGenerator::getSobelKernelGy();
	Mat laplacianFilter = KernelGenerator::getLaplaceKernel();

	Mat removeNoiseImage = my_image.removeNoise(gaussianBlur5x5);
	MyImage::showImage(removeNoiseImage, "removeNoise");

	my_image = MyImage(removeNoiseImage);

    Mat imageGx = my_image.applyConv2d(sobelGx);
    MyImage::showImage(imageGx, "Sobel_Gx");

    Mat imageGy = my_image.applyConv2d(sobelGy);
    MyImage::showImage(imageGy, "Sobel_Gy");

    //Mat result = ImageOperator::addMatAbs(imageGx, imageGy);
	Mat result = my_image.applyConv2d(laplacianFilter);
	MyImage::showImage(result, "final");
    return 0;
}