#include "image_operator.hpp"


Mat ImageOperator::EdgeDetectSobel(const Mat& sourceImage, int gaussSize, float gaussStd, int edge_thres, bool isShow){
    MyImage image(sourceImage);
	// remove noise image
	Mat gaussianMask = KernelGenerator::createGaussianKernel(gaussSize, gaussStd);
	Mat cleanedImage = image.removeNoise(gaussianMask);

	
	Mat sobelGx = KernelGenerator::getSobelKernelGx();
    Mat imageGx = conv2d(cleanedImage, sobelGx, false, false);

    Mat sobelGy = KernelGenerator::getSobelKernelGy();
    Mat imageGy = conv2d(cleanedImage, sobelGy, false, false);
    
    Mat sobelResult= addMatrix(imageGx, imageGy);
	maximizeEdgePixels(sobelResult, edge_thres);

	if (isShow){
		MyImage::showImageFromMatrix(sourceImage, "Input");
		MyImage::showImageFromMatrix(cleanedImage, "Blur", 0, sourceImage.rows+20);
		MyImage::showImageFromMatrix(imageGx, "Sobel Gx", sourceImage.cols, 0);
		MyImage::showImageFromMatrix(imageGy, "Sobel Gy", sourceImage.cols*2, 0);
		MyImage::showImageFromMatrix(sobelResult, "Sobel result", sourceImage.cols, sourceImage.rows+20);
	}

    return sobelResult;
}

Mat ImageOperator::EdgeDetectPrewitt(const Mat& sourceImage, int gaussSize, float gaussStd, int edge_thres, bool isShow){
	MyImage image(sourceImage);
	// remove noise image
	Mat gaussianMask = KernelGenerator::createGaussianKernel(gaussSize, gaussStd);
	Mat cleanedImage = image.removeNoise(gaussianMask);

    Mat prewittGx = KernelGenerator::getPrewittKernelGx();
    Mat imageGx = conv2d(cleanedImage, prewittGx, false, false);

    Mat prewittGy = KernelGenerator::getPrewittKernelGy();
    Mat imageGy = conv2d(cleanedImage, prewittGy, false, false);

    Mat prewittResult= addMatrix(imageGx, imageGy);
    
	maximizeEdgePixels(prewittResult, edge_thres);
	if (isShow){
		MyImage::showImageFromMatrix(sourceImage, "Input");
		MyImage::showImageFromMatrix(cleanedImage, "Blur", 0, sourceImage.rows+20);
		MyImage::showImageFromMatrix(imageGx, "Prewitt Gx", sourceImage.cols, 0);
		MyImage::showImageFromMatrix(imageGy, "Prewitt Gy", sourceImage.cols*2, 0);
		MyImage::showImageFromMatrix(prewittResult, "Prewitt result", sourceImage.cols, sourceImage.rows+20);
	}

    return prewittResult;
}

Mat ImageOperator::EdgeDetectCanny(const Mat& sourceImage, int gaussSize, float gaussStd, int low_thres, int high_thres, bool isShow) {
	MyImage image(sourceImage);
	// remove noise image
	Mat gaussianMask = KernelGenerator::createGaussianKernel(gaussSize, gaussStd);
	Mat cleanedImage = image.removeNoise(gaussianMask);
	

	Mat imageGx, imageGy, magnitude = Mat::zeros(sourceImage.rows, sourceImage.cols, CV_32FC1);
	Mat direction, canny_result = Mat::zeros(sourceImage.rows, sourceImage.cols, CV_8UC1);

	imageGx = ImageOperator::conv2d(cleanedImage, KernelGenerator::getSobelKernelGx(), true, true);
	imageGy = ImageOperator::conv2d(cleanedImage, KernelGenerator::getSobelKernelGy(), true, true);


	magnitude = computeMagnitude(imageGx, imageGy);


	direction = computeDirection(imageGx, imageGy);

	NonMaxSuppression(direction, magnitude);

	canny_result = HysteresisThresholding(magnitude, high_thres, low_thres);

    if (isShow){
        MyImage::showImageFromMatrix(sourceImage, "Input image", 0, 0);
        MyImage::showImageFromMatrix(canny_result, "Canny result", sourceImage.cols, 0);
		MyImage::showImageFromMatrix(cleanedImage, "blur", 0, sourceImage.rows+20);
    }
	return canny_result;
}

Mat ImageOperator::EdgeDetectLaplacian(const Mat& sourceImage, int gaussSize, float gaussStd, float max_thres, bool isShow) {
	MyImage image(sourceImage);

	// remove noise image
	Mat gaussianMask = KernelGenerator::createGaussianKernel(gaussSize, gaussStd);
	Mat cleanedImage = image.removeNoise(gaussianMask);
	
	// get Laplacian kernel
	Mat laplaceKernel = KernelGenerator::getLaplaceKernel();

	// use laplaceKernel to compute Laplacian gradient of source image
	Mat laplacianImage = ImageOperator::conv2d(cleanedImage, laplaceKernel, true, true);
	
	//cout << laplacianImage << endl;

	// get max value of laplacian matrix:
	int maxValue = ImageOperator::getMaxValue(laplacianImage);
	
	// define slope threshold
	float slopeThres = maxValue*max_thres;

	// find zero crossing points in laplacian matrix
	Mat zeroCrossingResultImage = ImageOperator::findZeroCrossingPoints(laplacianImage, slopeThres);
	
    if (isShow){
        MyImage::showImageFromMatrix(sourceImage, "input image", 0, 0);
        MyImage::showImageFromMatrix(zeroCrossingResultImage, "Laplacian result", sourceImage.cols, 0);
		MyImage::showImageFromMatrix(cleanedImage, "blur", 0, sourceImage.rows+20);
    }
    
	return zeroCrossingResultImage;
}


int ImageOperator::measureDifference(const Mat &result, const Mat &ground_truth) {
	if (result.rows != ground_truth.rows || result.cols != ground_truth.cols || result.channels() != ground_truth.channels())
		return -1;
	int diff = 0;
	int height = ground_truth.rows, width = ground_truth.cols;
	float eps = 1e-3;

	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
			diff += abs(getValueOfMatrix(result, y, x) - getValueOfMatrix(ground_truth, y, x)) > eps;
	return diff;
}

// ---------------------------------------------------------------------------------------------------
// Common helper functions
Mat ImageOperator::conv2d(const Mat& source, const Mat& kernel, bool acceptNegative, bool acceptExceed) {
	int sHeight = source.rows;
	int sWidth = source.cols;

	Mat result = Mat::zeros(sHeight, sWidth, CV_32FC1);

	for (int y = 0; y < sHeight; y++) {
		for (int x = 0; x < sWidth; x++) {
			float res = applyConvolutionAtPosition(source, x, y, kernel);
			if (acceptExceed == false) 
				res = res > 255 ? 255 : res;
			if (acceptNegative == false) 
				res = res < 0 ? 0 : res;
			setValueOfMatrix(result, y, x, res);
		}
	}
	return result;
}

float ImageOperator::applyConvolutionAtPosition(const Mat& source, int x, int y, const Mat& kernel) {
	int sWidth = source.cols;
	int sHeight = source.rows;

	int kHeight = kernel.rows;
	int kWidth = kernel.cols;

	int startSourceX = x + kWidth / 2;
	int startSourceY = y + kHeight / 2;

	float convResult = 0;

	for (int ky = 0; ky < kHeight; ++ky) {
		int sourceY = startSourceY - ky;

		for (int kx = 0; kx < kWidth; ++kx) {
			int sourceX = startSourceX - kx;

			if (sourceY < 0 || sourceY >= sHeight || sourceX < 0 || sourceX >= sWidth)
				continue;

			convResult += getValueOfMatrix(source, sourceY, sourceX) * getValueOfMatrix(kernel, ky, kx);
		}
	}
	return convResult;
}

// ---------------------------------------------------------------------------------------------------
// Laplacian helper functions
int ImageOperator::getMaxValue(const Mat& source){
    int result = -1;
    // TODO: find max value of matrix source
    int sHeight = source.rows;
    int sWidth = source.cols;
    for(int y = 0; y < sHeight; y++){
        for(int x = 0; x < sWidth; x++){
            int value = source.at<float>(y, x);
            result = result > value ? result : value;
        }
    }
    return result;
}
Mat ImageOperator::findZeroCrossingPoints(const Mat& source, float slopeThres){
    int sHeight = source.rows;
    int sWidth = source.cols;
    int dx[] = {-1, 0};
    int dy[] = {0, 1};

    Mat result = Mat::zeros(sHeight, sWidth, CV_8UC1);

    for(int y = 0; y < sHeight; y++){
        for(int x = 0; x < sWidth; x++){
            if (getValueOfMatrix(source, y, x) == 0)
                checkZeroBetween(source, result, y, x, slopeThres);
            else
                checkNonZeroBetween(source, result, y, x, slopeThres);
            
        }
    }

    return result;
}
void ImageOperator::checkNonZeroBetween(const Mat& source, Mat& result, int y, int x, float slopeThres){
    float currentPoint = getValueOfMatrix(source, y, x);// source.at<float>(y, x);
    
    // check rightward
    if (x <= source.cols-1){
        float rightPoint = getValueOfMatrix(source, y, x+1);//source.at<float>(y, x + 1);
        if (checkEdgePointCondition(currentPoint, rightPoint, slopeThres)){
            setValueOfMatrix(result, y, x, 255);
        }
    }
    
    //check downward
    if (y <= source.rows-1){
        float downPoint = getValueOfMatrix(source, y+1, x);
        if (checkEdgePointCondition(currentPoint, downPoint, slopeThres)){
            setValueOfMatrix(result, y, x, 255);
        }
    }
    
}
void ImageOperator::checkZeroBetween(const Mat& source, Mat& result, int y, int x, float slopeThres){
    int dx[] = {-1, 0};
    int dy[] = {0, 1};
    
    //compute local variance
    for(int k = 0; k < 2; k++){
        int x1 = x + dx[k];
        int y1 = y + dy[k];
        int x2 = x - dx[k];
        int y2 = y - dy[k];

        if (x1 < 0 || x1 >= source.cols || x2 < 0 || x2 >= source.cols ||
            y1 < 0 || y1 >= source.rows || y2 < 0 || y2 >= source.rows){
            continue;
        }

        float neighbor1 = getValueOfMatrix(source, y1, x1);
        float neighbor2 = getValueOfMatrix(source, y2, x2);
        
        if (checkEdgePointCondition(neighbor1, neighbor2, slopeThres)){
            setValueOfMatrix(result, y, x, 255);
        }
    }
}

bool ImageOperator::checkEdgePointCondition(float point1, float point2, float slopeThres) {
	int sign1 = point1 < 0 ? -1 : 1;
	int sign2 = point2 < 0 ? -1 : 1;
	return (sign1 != sign2) && (abs(point1) + abs(point2) > slopeThres);
}


// ---------------------------------------------------------------------------------------------------
// Canny helper functions
Mat ImageOperator::computeMagnitude(const Mat& a, const Mat& b) {
	int aHeight = a.rows;
	int aWidth = a.cols;
	Mat result = Mat::zeros(aHeight, aWidth, CV_32FC1);

	for (int y = 0; y < aHeight; y++) {
		for (int x = 0; x < aWidth; x++) {
			float value_a = getValueOfMatrix(a, y, x);
			float value_b = getValueOfMatrix(b, y, x);
			float sum = sqrt(value_a * value_a + value_b * value_b);
			setValueOfMatrix(result, y, x, sum);
		}
	}
	return result;
}

Mat ImageOperator::computeDirection(const Mat& gx, const Mat &gy) {
	int aHeight = gx.rows;
	int aWidth = gx.cols;
	Mat result = Mat::zeros(aHeight, aWidth, CV_8UC1);

	float PI = KernelGenerator::pi;

	for (int y = 0; y < aHeight; y++) {
		for (int x = 0; x < aWidth; x++) {
			float value_gy = getValueOfMatrix(gy, y, x), value_gx = getValueOfMatrix(gx, y, x);
			int angle = (int)(atan2(value_gy, value_gx) * 180 / PI);
			if (angle < 0) angle += 180;
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
			setValueOfMatrix(result, y, x, angle); //0, 45, 90, 135
		}
	}
	return result;
}
void ImageOperator::NonMaxSuppression(const Mat &direction, Mat &gradient) {
	int aHeight = direction.rows;
	int aWidth = direction.cols;
	
	int angle, compare = 0;
	float current_gradient;

	for (int y = 0; y < aHeight; ++y) {
		for (int x = 0; x < aWidth; ++x) {
			angle = (int)getValueOfMatrix(direction, y, x);
			current_gradient = getValueOfMatrix(gradient, y, x);
			compare = 0;

			float gradient_compare_1 = -1e8, gradient_compare_2 = -1e8;
			
			if (angle == 0) //compare gradient at (y, x) with (y, x + 1) & (y, x - 1)
			{
				gradient_compare_1 = (x + 1 < aWidth) ? getValueOfMatrix(gradient, y, x + 1) : (-1e8);
				gradient_compare_2 = (x - 1 >= 0) ? getValueOfMatrix(gradient, y, x - 1) : (-1e8);
			}
			else if (angle == 45) //compare gradient at (y, x) with (y - 1, x + 1) & (y + 1, x - 1)
			{
				gradient_compare_1 = ((x + 1 < aWidth) && (y - 1 >= 0)) ? getValueOfMatrix(gradient, y - 1, x + 1) : (-1e8);
				gradient_compare_2 = ((x - 1 >= 0) && (y + 1 < aHeight)) ? getValueOfMatrix(gradient, y + 1, x - 1) : (-1e8);
			}
			else if (angle == 90) //compare gradient at (y, x) with (y - 1, x) & (y + 1, x)
			{
				gradient_compare_1 = (y + 1 < aHeight) ? getValueOfMatrix(gradient, y + 1, x) : (-1e8);
				gradient_compare_2 = (y - 1 >= 0) ? getValueOfMatrix(gradient, y - 1, x) : (-1e8);
			}
			else if (angle == 135) //compare gradient at (y, x) with (y - 1, x - 1) & (y + 1, x + 1)
			{
				gradient_compare_1 = ((x - 1 >= 0) && (y - 1 >= 0)) ? getValueOfMatrix(gradient, y - 1, x - 1) : (-1e8);
				gradient_compare_2 = ((x + 1 < aWidth) && (y + 1 < aHeight)) ? getValueOfMatrix(gradient, y + 1, x + 1) : (-1e8);
			}
			compare += current_gradient >= gradient_compare_1;
			compare += current_gradient >= gradient_compare_2;
			if (compare < 2)
				setValueOfMatrix(gradient, y, x, (float)0);
		}
	}
}
void ImageOperator::dfs(Mat &canny_mask, const Mat &gradient, int y, int x, float low_threshold, vector<vector<bool>> &visited) { //dfs at position (y, x)
	int height = canny_mask.rows, width = canny_mask.cols;

	visited[y][x] = true;
	canny_mask.at<uchar>(y, x) = 255;

	for (int step_y = -1; step_y <= 1; ++step_y) {
		for (int step_x = -1; step_x <= 1; ++step_x) {
			if (y + step_y >= height || y + step_y < 0 || x + step_x >= width || x + step_x < 0)
				continue;
			if (visited[y + step_y][x + step_x] == true)
				continue;
			
			float gradient_neighbor = getValueOfMatrix(gradient, y + step_y, x + step_x);

			if (gradient_neighbor >= low_threshold) {
				dfs(canny_mask, gradient, y + step_y, x + step_x, low_threshold, visited);
			}
			//cout << "Loop" << " " << y << " " << x << " " << y + step_y << " " << x + step_x << endl;
		}
	}
}
Mat ImageOperator::HysteresisThresholding(const Mat &gradient, float high_threshold, float low_threshold) {
	//assert(done the non-max suppression step!)
	int aHeight = gradient.rows;
	int aWidth = gradient.cols;
	Mat canny_mask = Mat::zeros(aHeight, aWidth, CV_8UC1);

	for (int y = 0; y < aHeight; ++y) {
		for (int x = 0; x < aWidth; ++x) {
			if (getValueOfMatrix(gradient, y, x) < low_threshold)
				canny_mask.at<uchar>(y, x) = 0;
			else if (getValueOfMatrix(gradient, y, x) > high_threshold)
				canny_mask.at<uchar>(y, x) = 255;
		}
	}

	vector<vector<bool>> visited(aHeight, vector<bool>(aWidth, false));

	for (int y = 0; y < aHeight; ++y){
		for (int x = 0; x < aWidth; ++x){
			if (canny_mask.at<uchar>(y, x) == 255 && visited[y][x] == false){
				dfs(canny_mask, gradient, y, x, low_threshold, visited);
			}
		}
	}

	return canny_mask;
}


// ---------------------------------------------------------------------------------------------------
// Refinement helper functions
void ImageOperator::maximizeEdgePixels(Mat& source, int thres){
	int height = source.rows;
	int width = source.cols;
	for(int y = 0; y < height; y++){
		for(int x = 0; x < width; x++){
			if (getValueOfMatrix(source, y, x) > thres){
				setValueOfMatrix(source, y, x, 255);
			}
			else{
				setValueOfMatrix(source, y, x, 0);
			}
		}
	}

}