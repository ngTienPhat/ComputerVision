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
            float xySigma = (float)(x*x + y*y)/(2*kernelSigma*kernelSigma);
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

Mat KernelGenerator::createGaussianKernel(int gaussSize, float gaussStd){
    Mat gaussKernel = Mat::zeros(gaussSize, gaussSize, CV_32FC1);
    float sum = 0.0;
    float var = 2*gaussStd*gaussStd;
    float r;
    
    for(int y = -(gaussSize/2); y <= gaussSize/2 ; y++){
        for(int x = -(gaussSize/2); x <= gaussSize/2; x++){
            r = sqrt(x*x + y*y);
            gaussKernel.at<float>(y + gaussSize/2, x + gaussSize/2) = exp(-(r*r)/var) / (pi*var);
            sum += gaussKernel.at<float>(y + gaussSize/2, x + gaussSize/2);
        }
    }

    for(int i = 0; i < gaussSize; i++){
        for(int j = 0; j < gaussSize; j++){
            gaussKernel.at<float>(i, j)/=sum;
        }
    }

    return gaussKernel;
}