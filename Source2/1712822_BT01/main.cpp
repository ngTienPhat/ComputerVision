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
	Mat prewittGx = KernelGenerator::getPrewittKernelGx();
	Mat prewittGy = KernelGenerator::getPrewittKernelGy();
	Mat laplacianFilter = KernelGenerator::getLaplaceKernel();

	Mat removeNoiseImage = my_image.removeNoise(gaussianBlur3x3);
	MyImage::showImage(removeNoiseImage, "removeNoise");
	
	MyImage removeNoiseGrayscale(removeNoiseImage);

    Mat imageGx = removeNoiseGrayscale.applyConv2d(prewittGx);
    MyImage::showImage(imageGx, "Sobel_Gx");

	Mat imageGy = removeNoiseGrayscale.applyConv2d(prewittGy);
	MyImage::showImage(imageGx, "Sobel_Gy");

	Mat magnitude = ImageOperator::magnitude(imageGx, imageGy);
	MyImage::showImage(magnitude, "Sobel");

	Mat direction = ImageOperator::computeDirection(imageGx, imageGy);

	ImageOperator::NonMaxSuppression(direction, magnitude);
	Mat canny_result = ImageOperator::HysteresisThresholding(magnitude, 100, 10);
	MyImage::showImage(canny_result, "Canny");

    return 0;
}	