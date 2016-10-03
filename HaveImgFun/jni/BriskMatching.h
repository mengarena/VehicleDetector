//#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/legacy/legacy.hpp>

#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
//#include "features2d.hpp"
//#include "nonfree.hpp"

using namespace std;
using namespace cv;

class BriskMatching
{
private:
	float ratio;	//Max ratio between 1st and 2nd NN
	double distance;	//Min distance to epipolar
	double confidence;	//Confidence level (probability)
	bool refineF;	//If true will refine the Fundamental matrix

public:
	BriskMatching(void);
	~BriskMatching(void);

	void setRatio(float rt);
	void setDistance(double dst);
	void setConfidence(double confd);

	int ratioTest(vector< vector< DMatch > >& matches);
	void symmetryTest(const vector< vector< DMatch > >& matches1,const vector< vector< DMatch > >& matches2, vector< DMatch >& symMatches);
	void ransacTest(const vector< DMatch >& matches, const vector< KeyPoint >& keypoints1, const vector< KeyPoint >& keypoints2, vector< DMatch >& outMatches);
	vector<DMatch> DoMatching(Mat &image1, Mat &image2, double &time);

	void filterWithSlopeDistance(Mat& image1,
						Mat& image2,
						const vector< KeyPoint >& keypoints1,
						const vector< KeyPoint >& keypoints2,
						const vector< DMatch >& inMatches,
						vector< DMatch >& finalMatches);

};

