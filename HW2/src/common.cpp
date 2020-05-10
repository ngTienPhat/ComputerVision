#include "common.hpp"

void printMatrixInfo(const Mat &source){
    int typeMatrix = source.type();
    string printOut;

    uchar depth = typeMatrix & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (typeMatrix >> CV_CN_SHIFT);

    switch(depth){
        case CV_8U:  printOut = "8U"; break;
        case CV_8S:  printOut = "8S"; break;
        case CV_16U: printOut = "16U"; break;
        case CV_16S: printOut = "16S"; break;
        case CV_32S: printOut = "32S"; break;
        case CV_32F: printOut = "32F"; break;
        case CV_64F: printOut = "64F"; break;
        default:     printOut = "User"; break;
    }

    printOut += "C";
    printOut += (chans+'0');

    cout << printOut << " " << source.rows << "x" << source.cols << endl;
}
