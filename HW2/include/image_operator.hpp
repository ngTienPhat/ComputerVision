#ifndef IMAGE_OPERATOR_HPP__
#define IMAGE_OPERATOR_HPP__

#include "common.hpp"

class ImageOperator{


// PUBLIC FUNCTIONS
public:

    static Mat conv2d(const Mat& source, const Mat& kernel, string padding="same", int stride=1){
        int sHeight = source.rows;
        int sWidth= source.cols;
        int sChannel = source.channels();

        Mat sClone = source.clone();

        for(int y = 0; y < sHeight; y++){
            for(int x = 0; x < sWidth; x++){
                int res = applyConvolutionAtPosition(source, x, y, kernel);
                res = res > 255 ? 255:res;
                res = res < 0 ? 0 : res;
                sClone.at<uchar>(y, x) = res;
            }
        }
        return sClone;
    }

    static Mat addMatAbs(const Mat& a, const Mat& b){
        Mat result = a.clone();
        int aHeight = a.rows;
        int aWidth = a.cols;

        for(int y = 0; y < aHeight; y++){
            for(int x = 0; x < aWidth; x++){
                int sum = a.at<uchar>(y, x) + b.at<uchar>(y, x);
                sum = sum > 255 ? 255:sum;
                sum = sum < 0 ? 0 : sum;
                result.at<uchar>(y, x) = sum;
            }
        }
        return result;
    }


// HELPER FUNCTION:
private:
    static int applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel){
        int sWidth = source.cols;
        int sHeight = source.rows;

        int kHeight = kernel.rows;
        int kWidth = kernel.cols;

        int startSourceX = x + kWidth/2;
        int startSourceY = y + kHeight/2;

        int convResult = 0;
        for(int ky = 0; ky < kHeight; ky++){
            int sourceY = startSourceY - ky;

            for(int kx = 0; kx < kWidth; kx++){
                int sourceX =  startSourceX - kx;

                if (sourceY < 0 || sourceY >= sHeight ||sourceX < 0 ||sourceX >= sWidth)
                    continue;

                convResult += source.at<uchar>(startSourceY - ky, startSourceX - kx) * kernel.at<int>(ky, kx);
            }
        }

        return convResult;
    }

};

#endif //IMAGE_OPERATOR_HPP__