#include "kernel_generator.hpp"

const float KernelGenerator::pi = 3.14159265;

Mat KernelGenerator::createLoGkernel(int kernelSize, float kernelSigma){
    Mat logKernel = Mat::zeros(kernelSize, kernelSize, CV_32FC1);
    float sum = 0.0;
    float var = 2*kernelSigma*kernelSigma;
    float r;
    
    for(int y = -(kernelSize/2); y <= kernelSize/2 ; y++){
        for(int x = -(kernelSize/2); x <= kernelSize/2; x++){
            r = sqrt(x*x + y*y);
            float xySigma = (x*x + y*y)/(2*kernelSigma*kernelSigma);
            float value = (1.0/(pi*pow(kernelSigma, 4))) * (1 - xySigma) * exp(-xySigma);
            logKernel.at<float>(y + kernelSize/2, x + kernelSize/2) = value;
            sum += logKernel.at<float>(y + kernelSize/2, x + kernelSize/2);
        }
    }

    for(int i = 0; i < kernelSize; i++){
        for(int j = 0; j < kernelSize; j++){
            logKernel.at<float>(i, j)/=sum;
        }
    }

    return logKernel;
}