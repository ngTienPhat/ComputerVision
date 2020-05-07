#include "image.hpp"
#include "image_operator.hpp"
#include "kernel_generator.hpp"

int main(){
    string image_dir = "lena.png";

    MyImage my_image = MyImage(image_dir, IMREAD_GRAYSCALE);    

    Mat sobelGx = KernelGenerator::getSobelKernelGx();
    Mat sobelGy = KernelGenerator::getSobelKernelGy();

    Mat imageGx = my_image.applyConv2d(sobelGx);
    MyImage::showImage(imageGx, "Sobel_Gx");

    Mat imageGy = my_image.applyConv2d(sobelGy);
    MyImage::showImage(imageGy, "Sobel_Gy");

    Mat result = ImageOperator::addMatAbs(imageGx, imageGy);
    MyImage::showImage(result, "final");
    return 0;
}