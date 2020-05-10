#ifndef IMAGE_OPERATOR_HPP__
#define IMAGE_OPERATOR_HPP__

#include "common.hpp"

class ImageOperator{

// PUBLIC FUNCTIONS
public:
    static Mat conv2d(const Mat& source, const Mat& kernel, string padding="same", int stride=1, bool useFloat=false){
        int sHeight = source.rows;
        int sWidth= source.cols;
        int sChannel = source.channels();

        Mat sClone = source.clone();

        for(int y = 0; y < sHeight; y++){
            for(int x = 0; x < sWidth; x++){
				int res = applyConvolutionAtPosition(source, x, y, kernel, useFloat);
                res = res > 255 ? 255:res;
                res = res < 0 ? 0 : res;
                sClone.at<uchar>(y, x) = res;
            }
        }
        return sClone;
    }

    static Mat magnitude(const Mat& a, const Mat& b){
        Mat result = a.clone();
        int aHeight = a.rows;
        int aWidth = a.cols;

        for(int y = 0; y < aHeight; y++){
            for(int x = 0; x < aWidth; x++){
                int sum = (int) sqrt(double(a.at<uchar>(y, x) * a.at<uchar>(y, x) + b.at<uchar>(y, x) * b.at<uchar>(y, x)));
                sum = sum > 255 ? 255:sum;
                sum = sum < 0 ? 0 : sum;
                result.at<uchar>(y, x) = sum;
            }
        }
        return result;
    }
	static Mat computeDirection(const Mat& gx, const Mat &gy) {
		Mat result = gx.clone();
		int aHeight = result.rows;
		int aWidth = result.cols;

		double PI = 3.14159265;

		for (int y = 0; y < aHeight; y++) {
			for (int x = 0; x < aWidth; x++) {
				double angle = atan2((double)gy.at<uchar>(y,x),(double) gx.at<uchar>(y, x)) * 180 / PI;
				if (angle <= 22.5)
					angle = 0;
				else if (angle <= 67.5)
					angle = 45;
				else if (angle <= 112.5)
					angle = 90;
				else if (angle <= 157.5)
					angle = 135;
				else //angle > 157.5
					angle = 0;
				result.at<uchar>(y, x) = angle; //0, 45, 90, 135
			}
		}
		return result;
	}
	static void NonMaxSuppression(const Mat &direction, Mat &gradient) {
		int aHeight = direction.rows;
		int aWidth = direction.cols;

		for (int y = 0; y < aHeight; ++y){
			for (int x = 0; x < aWidth; ++x) {
				int angle = direction.at<uchar>(y, x);
				uchar current_gradient = gradient.at<uchar>(y, x);
				int compare = 0;

				if (angle == 0) //compare gradient at (y, x) with (y, x + 1) & (y, x - 1)
				{ 
					compare += (x + 1 < aWidth) ? 
									(current_gradient >= gradient.at<uchar>(y, x + 1)) : 1;
					compare += (x - 1 >= 0) ? 
									(current_gradient >= gradient.at<uchar>(y, x - 1)) : 1;
				}
				else if (angle == 45) //compare gradient at (y, x) with (y - 1, x + 1) & (y + 1, x - 1)
				{
					compare += ((x + 1 < aWidth) && (y - 1 >= 0)) ? 
									(current_gradient >= gradient.at<uchar>(y - 1, x + 1)) : 1;
					compare += ((x - 1 >= 0) && (y + 1 < aHeight)) ? 
									(current_gradient >= gradient.at<uchar>(y + 1, x - 1)) : 1;
				}
				else if (angle == 90) //compare gradient at (y, x) with (y - 1, x) & (y + 1, x)
				{ 
					compare += (y + 1 < aHeight) ? 
									(current_gradient >= gradient.at<uchar>(y + 1, x)) : 1;
					compare += (y - 1 >= 0) ? 
									(current_gradient >= gradient.at<uchar>(y - 1, x)) : 1;
				}
				else if (angle == 135) //compare gradient at (y, x) with (y - 1, x - 1) & (y + 1, x + 1)
				{ 
					compare += ((x - 1 >= 0) && (y - 1 >= 0)) ?
									(current_gradient >= gradient.at<uchar>(y - 1, x - 1)) : 1;
					compare += ((x + 1 < aWidth) && (y + 1 < aHeight)) ?
									(current_gradient >= gradient.at<uchar>(y + 1, x + 1)) : 1;
				}
				if (compare < 2)
					gradient.at<uchar>(y, x) = 0;
			}
		}
	}
	static void dfs(Mat &canny_mask, const Mat &gradient, int y, int x, double high_threshold, double low_threshold, vector<vector<bool>> &visited) { //dfs at position (y, x)
		int height = canny_mask.rows, width = canny_mask.cols;

		visited[y][x] = true;
		canny_mask.at<uchar>(y, x) = 255;

		for (int step_y = -1; step_y <= 1; ++step_y) {
			for (int step_x = -1; step_x <= 1; ++step_x) {
				if (y + step_y >= height || y + step_y < 0 || x + step_x >= width || x + step_x < 0)
					continue;
				if (visited[y + step_y][x + step_x] == true)
					continue;
				if (gradient.at<uchar>(y + step_y, x + step_x) >= low_threshold)
				{
					dfs(canny_mask, gradient, y + step_y, x + step_x, high_threshold, low_threshold, visited);
				}
			}
		}
	}

	static Mat HysteresisThresholding(const Mat &gradient, double high_threshold, double low_threshold) {
		//assert(done the non-max suppression step!)
		Mat canny_mask = gradient.clone();
		int aHeight = canny_mask.rows;
		int aWidth = canny_mask.cols;

		for (int y = 0; y < aHeight; ++y)
			for (int x = 0; x < aWidth; ++x)
				canny_mask.at<uchar>(y, x) = 0;

		for (int y = 0; y < aHeight; ++y) {
			for (int x = 0; x < aWidth; ++x) {
				if (gradient.at<uchar>(y, x) < low_threshold)
					canny_mask.at<uchar>(y, x) = 0;
				else if (gradient.at<uchar>(y, x) > high_threshold)
					canny_mask.at<uchar>(y, x) = 255;
			}
		}

		vector<vector<bool>> visited(aHeight, vector<bool>(aWidth, false));

		for (int y = 0; y < aHeight; ++y) 
			for (int x = 0; x < aWidth; ++x) 
				if (canny_mask.at<uchar>(y, x) == 255 && visited[y][x] == false)
					dfs(canny_mask, gradient, y, x, high_threshold, low_threshold, visited);

		return canny_mask;
	}

// HELPER FUNCTION:
private:
    static int applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel, bool useFloat=false){
        int sWidth = source.cols;
        int sHeight = source.rows;

        int kHeight = kernel.rows;
        int kWidth = kernel.cols;

        int startSourceX = x + kWidth/2;
        int startSourceY = y + kHeight/2;

        int convResult_int = 0;
		float convResult_float = 0;

        for(int ky = 0; ky < kHeight; ++ky){
            int sourceY = startSourceY - ky;

            for(int kx = 0; kx < kWidth; ++kx){
                int sourceX =  startSourceX - kx;

                if (sourceY < 0 || sourceY >= sHeight ||sourceX < 0 ||sourceX >= sWidth)
                    continue;

				if (useFloat == false)
					convResult_int += (int)source.at<uchar>(sourceY, sourceX) * kernel.at<int>(ky, kx);
				else
					convResult_float += 1.0*source.at<uchar>(sourceY, sourceX) * kernel.at<float>(ky, kx);
            }
        }
        return (useFloat==true)?((int)convResult_float):convResult_int;
    }

};

#endif //IMAGE_OPERATOR_HPP__