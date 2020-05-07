/**
*    author:  Khanh-Toan Nguyen, 1712822, 17TN, HCMUS.
*    created: 07.05.2020 18:45:29
**/

#ifndef sobelFiltering_h
#define sobelFiltering_h

#include"Utils.h"

int xGradient_Sobel(const Mat &image, int x, int y);
int yGradient_Sobel(const Mat &image, int x, int y);
int detectBySobel(const Mat &src, Mat &dst, Mat &gxMat, Mat &gyMat);


#endif // !sobelFiltering_h
