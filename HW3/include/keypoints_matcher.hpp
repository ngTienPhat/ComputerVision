#pragma once
#include "corner_detector.hpp"
#include "common.hpp"
#include "sift.hpp"

struct PairKeypoint{
    Extrema pointTrain;//store keypoint from train image closest to test image below
    Extrema pointTest;//store keypoint from test image
    float distance;
};

class KeypointsMatcher{
private:
    int K = 1;

    // sift parameter:
    float siftBaseSigma = 1.6;
    int siftNumOctaves = 4;
    int siftNumDOGperOctave = 3;

public:
    // 
    vector<PairKeypoint> matching(const string &imageTrain, const string& imageTest);

private:
    // init matrix of all keypoints detected in train image
    Mat initTrainMatrix(const vector<Extrema> &keypoints);
    
    Mat getTrainLabels(const vector<Extrema> &keypoints);

    // get closest keypoint
    PairKeypoint getClosestKeypoint(const Mat &trainMatrix, const Mat& trainLabels, Extrema& testKp, const vector<Extrema>& listTrainKp);

    // visualize result
    void visualizeMatchingResult(const vector<PairKeypoint> &result, const string& imageTrain, const string& imageTest);
};
