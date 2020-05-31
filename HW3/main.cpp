/**
*    author:  Khanh-Toan Nguyen, 1712822, 17TN, HCMUS.
*    created: 20.05.2020 15:40:30
**/

#include"Harris.h"
#include"Blob.h"
#include"Sift.h"

int main(int argc, char** argv)
{
	string image_name_train = "download", image_name_test = "02_3";
	string image_type_train = "png", image_type_test = "jpg";

	Mat src_train = imread(image_name_train + '.' + image_type_train, IMREAD_COLOR);
	cout << "The input train_image information: ";
	printMatrixInfo(src_train);

	Mat src_test = imread(image_name_test + '.' + image_type_test, IMREAD_COLOR);
	cout << "The input test_image information: ";
	printMatrixInfo(src_test);

	BlobDetector blobDetector;
	HarrisDetector harrisDetector;
	SiftDetector siftDetector;

	Mat src = imread("butterfly.png", IMREAD_COLOR);
	imshow("srcImage", src);

	/* 1. Detect Corner using Harris Detector */
	//Mat src = imread("lena.png", IMREAD_COLOR);
	//imshow("srcImage", src_train);
	//vector<CornerPoint> corners = harrisDetector.detectHarris(src_train);
	//harrisDetector.showCornerPoint(src_train, corners);

	/* 2. Detect Blob using Blob, DoG Detector (blob slace space detection) */
	set<tuple<int,int,float>> blob_pts = blobDetector.detectBlob(src, 1, sqrt(2), 0.4);
	blobDetector.showBlobPoint_BlobDetector(src, blob_pts);

	blob_pts = blobDetector.detectDOG(src);
	blobDetector.showBlobPoint_DOGDetector(src, blob_pts);

	/* 3. Extrace SIFT features from Image */
	//if (siftDetector.matchingTwoImages(src_train, src_test, 1))
	//	cout << "Matching" << endl;

	//siftDetector.siftDetector(src_test);

	return 0;
}
