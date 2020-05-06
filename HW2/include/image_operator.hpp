#ifndef IMAGE_OPERATOR_HPP__
#define IMAGE_OPERATOR_HPP__

#include "common.hpp"

class ImageOperator{


// PUBLIC FUNCTIONS
public:

    static Mat conv2d(const Mat& source, const Mat& kernel, string padding="same", int stride=1){
        int kHeight = kernel.rows;
        int kWidth = kernel.cols;
        int sHeight = source.rows;
        int sWidth= source.cols;
        int sChannel = source.channels();

        Mat sClone = source.clone();

        for(int y = 0; y < sHeight; y++){
            for(int x = 0; x < sWidth; x++){
                sClone.at<float>(y, x, 0) = applyConvolutionAtPosition(source, x, y, kernel);
            }
        }
        return sClone;
    }


// HELPER FUNCTION:
private:
    static float applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel){
        int sWidth = source.cols;
        int sHeight = source.rows;

        int kHeight = kernel.rows;
        int kWidth = kernel.cols;
        int kCenterX = kWidth/2;
        int kCenterY = kHeight/2;

        int startSourceX = x - kCenterX;
        int startSourceY = y - kCenterY;

        float convResult = 0;
        for(int ky = 0; ky < kHeight; ky++){
            int kRowIndex = kHeight-1-ky;
            if (startSourceY + ky < 0 || startSourceY + ky >= sHeight){
                continue;
            }

            for(int kx = 0; kx < kWidth; kx++){
                int kColIndex = kWidth-1-kx;

                if (startSourceX + kx >= 0 && startSourceX + kx < sWidth){
                    convResult += source.at<float>(startSourceY + ky, startSourceX + kx, 0) * kernel.at<float>(kRowIndex, kColIndex, 0);
                }
            }
        }

        return convResult;
    }
};

#endif //IMAGE_OPERATOR_HPP__