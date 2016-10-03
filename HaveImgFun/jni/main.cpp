//Do image matching

#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\nonfree\features2d.hpp>
#include <opencv2\legacy\legacy.hpp>
#include <opencv2\nonfree\nonfree.hpp>

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include "SurfMatching.h"
#include "SiftMatching.h"

using namespace std;
using namespace cv; 

//String baseImageFile = "E:\\TestVideo\\Set1\\SelfTook.jpg";
//String receivedImageFile = "E:\\TestVideo\\Set1\\SelfTook_33.png";

int main()
{

//	Mat baseImage = imread(baseImageFile);
//	Mat receivedImage = imread(receivedImageFile);
	Mat baseImage = imread(baseImageFile, CV_LOAD_IMAGE_GRAYSCALE);
	Mat receivedImage = imread(receivedImageFile, CV_LOAD_IMAGE_GRAYSCALE);

	//Convert to grayscale
	//cvtColor(baseImage, baseImage, CV_BGR2GRAY);
	//cvtColor(receivedImage, receivedImage, CV_BGR2GRAY);


	/////////////////////////////////////////////////////////
#if 0
	//Matching with Surf
	SurfMatching myMatcher;
	vector<DMatch> myMatches;
#else
	//Matching with Sift
	SiftMatching myMatcher;
	vector<DMatch> myMatches;
#endif

	double processTime = 0.0;
	double duration; //Duration for image matching
	duration = static_cast<double>(getTickCount());

//	myMatches = myMatcher.DoMatching(baseImage, receivedImage);
//	myMatches = myMatcher.DoMatching(baseImage, baseImage);
	myMatches = myMatcher.DoMatching(baseImage, receivedImage, processTime);

//	myMatches = myMatcher.DoMatchingSimple(baseImage, receivedImage);

	duration = static_cast<double>(getTickCount())-duration;
	duration /= getTickFrequency(); // the elapsed time in s

	double fDistance = 0.0;
	double fDistance20 = 0.0;
	for(int i = 0; i<myMatches.size(); i++) {
		fDistance = fDistance + myMatches.at(i).distance;
		if (i<20) {
			fDistance20 = fDistance20 + myMatches.at(i).distance;
		}
	}

	cout<<"Matched:"	<<myMatches.size()<<endl;
	cout<<"Total Distance:	"<<fDistance<<endl;
	cout<<"Total First 20 Distance:	"<<fDistance20<<endl;
	cout<<"Total processing time:	"<<	duration*1000.0 <<"	ms"<<endl;
	cout<<"Process Time:	"<<processTime*1000.0<<"	ms"<<endl;

	getchar();

	return 0;
}




#if 0
	Mat image = imread(selfTakeImage);
	vector<KeyPoint> keypointsSurf, keypointsSift;
	Mat featureImageSurf, featureImageSift;
	double durationSurf, durationSift; 

	//SURF method
//	SurfFeatureDetector surf(400.);
	SurfFeatureDetector surf; //All default parameters

	durationSurf = static_cast<double>(getTickCount());
	surf.detect(image, keypointsSurf);
	durationSurf = static_cast<double>(getTickCount())-durationSurf;
	durationSurf /= getTickFrequency();

	drawKeypoints(image, keypointsSurf, featureImageSurf, Scalar(0,255,0), DrawMatchesFlags::DEFAULT);

	namedWindow("SURF");
	imshow("SURF",featureImageSurf);
	
	//SIFT method
//	SiftFeatureDetector sift(0.03, 10.);
	SiftFeatureDetector sift; //All default parameters

	durationSift = static_cast<double>(getTickCount());
	sift.detect(image, keypointsSift);
	durationSift = static_cast<double>(getTickCount())-durationSift;
	durationSift /= getTickFrequency();

	drawKeypoints(image, keypointsSift, featureImageSift, Scalar(0,255,0), DrawMatchesFlags::DEFAULT);

	namedWindow("SIFT");
	imshow("SIFT",featureImageSift);

	cout<<"SURF Feature Points:	"<<keypointsSurf.size()<<endl;
	cout<<"SIFT Feature Points:	"<<keypointsSift.size()<<endl;
	cout<<endl;
	cout<<"SURF Time:	"<<durationSurf*1000.0<<"	ms"<<endl;
	cout<<"SIFT Time:	"<<durationSift*1000.0<<"	ms"<<endl;

	waitKey(30000);

	return 0;

#endif
