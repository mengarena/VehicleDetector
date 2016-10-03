#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>
#include <sstream>

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/nonfree/nonfree.hpp>
//#include "features2d.hpp"
//#include "nonfree.hpp"

#include "SurfMatching.h"
#include "SiftMatching.h"
#include "BriskMatching.h"
#include "FreakMatching.h"
#include "OrbMatching.h"

using namespace cv;
using namespace std;

extern "C"
{

	double test_compareAlgorithm()
	{
		ofstream logFile;
		string sFilePathPre = "/mnt/sdcard/ForChooseMatchAlgorithm/Rear_";
		stringstream  baseImageFile;
		stringstream  receivedImageFile;
		int distArr[] = {20, 30, 40, 60};
		double processTime = 0.0;
		double duration; //Duration for image matching
		double dTotalDuration;
		double dAvgDuration;

		int nMatchCount;
		int nAvgMatchCount;
		Mat baseImage;
		Mat receivedImage;
		char strFilenameBase[100];
		char strFilenameRecv[100];
		vector< DMatch > myMatches;

//		SurfMatching myMatcher;		//nMatchIndex == 1
//		SiftMatching myMatcher;		//nMatchIndex == 2
//		BriskMatching myMatcher;	//nMatchIndex == 3
		FreakMatching myMatcher;	//nMatchIndex == 4
//		OrbMatching myMatcher;	//nMatchIndex == 5

		int nMatchIndex = 4;

		logFile.open("/mnt/sdcard/ForChooseMatchAlgorithm/IM_Time.csv", ofstream::out | ofstream::app);

		if (nMatchIndex == 1) {
			logFile<<"----------SURF----------"<<"\n";
		} else if (nMatchIndex == 2) {
			logFile<<"----------SIFT----------"<<"\n";
		} else if (nMatchIndex == 3) {
			logFile<<"----------SURF/Brisk----------"<<"\n";
		} else if (nMatchIndex == 4) {
			logFile<<"----------SURF/Freak----------"<<"\n";
		} else if (nMatchIndex == 5) {
			logFile<<"----------ORB----------"<<"\n";
		}

		for(int i = 0; i<=3; i++) {
//			baseImageFile = sFilePathPre + itoa(distArr[i]) + "feet.jpg";
//			baseImageFile<<sFilePathPre<<distArr[i]<<"feet.jpg";
			memset(strFilenameBase, 0x00, 100);
			sprintf(strFilenameBase, "%s%d%s", "/mnt/sdcard/ForChooseMatchAlgorithm/image/Rear_", distArr[i], "feet.jpg");
			baseImage = imread(strFilenameBase, CV_LOAD_IMAGE_GRAYSCALE);

			for(int j = i; j<=3; j++) {
//				receivedImageFile = sFilePathPre + distArr[j] + "feet.jpg";
//				receivedImageFile<<sFilePathPre<<distArr[j]<<"feet.jpg";
				memset(strFilenameRecv, 0x00, 100);
				sprintf(strFilenameRecv, "%s%d%s","/mnt/sdcard/ForChooseMatchAlgorithm/image/Rear_", distArr[j], "feet.jpg");

				receivedImage = imread(strFilenameRecv, CV_LOAD_IMAGE_GRAYSCALE);

			//	dTotalDuration = 0.0;
			//	dAvgDuration = 0.0;
			//	nMatchCount = 0;
			//	nAvgMatchCount = 0;

		//		for (int k = 0; k < 10; k++) {

					processTime = 0.0;
					duration = 0.0;

					duration = static_cast<double>(getTickCount());

					for (int k=1; k<=10; k++) {
						//	myMatches = myMatcher.DoMatching(baseImage, receivedImage);
						//	myMatches = myMatcher.DoMatching(baseImage, baseImage);
						myMatches = myMatcher.DoMatching(baseImage, receivedImage, processTime);
					}

					duration = static_cast<double>(getTickCount())-duration;
					duration = duration * 1.0/getTickFrequency();  // the elapsed time in s

					duration =  duration*1000.0/10.0;  // time in ms
					//nMatchCount = nMatchCount + myMatches.size();
					//dTotalDuration = dTotalDuration + duration;
		//		}

				//dAvgDuration = dTotalDuration/10.0;
				//nAvgMatchCount = nMatchCount/10;

//				logFile<<distArr[i]<<","<<distArr[j]<<","<<nAvgMatchCount<<","<<dAvgDuration<<"\n";
				logFile<<i<<","<<j<<","<<myMatches.size()<<","<<duration<<"\n";

			}
		}


		logFile.close();

		return 0.0;

//		double fDistance = 0.0;
//		double fDistance20 = 0.0;
//		for(int i = 0; i<myMatches.size(); i++) {
//			fDistance = fDistance + myMatches.at(i).distance;
//			if (i<20) {
//				fDistance20 = fDistance20 + myMatches.at(i).distance;
//			}
//		}

//		int nCount;
//		nCount = myMatches.size();
//		return nCount;

	}

#if 1
	void test_ImageMatch()
	{

		ofstream logFile;
		string sFilePathPre = "/mnt/sdcard/SkyEyeImage/Rear_";
		stringstream  baseImageFile;
		stringstream  receivedImageFile;
		int distArr[] = {20, 30, 40, 60};
		double processTime = 0.0;
		double duration; //Duration for image matching
		double dTotalDuration;
		double dAvgDuration;

		int nMatchCount;
		int nAvgMatchCount;
		Mat baseImage;
		Mat receivedImage;
		char strFilenameBase[100];
		char strFilenameRecv[100];
		vector< DMatch > myMatches;

//		SurfMatching myMatcher;		//nMatchIndex == 1
//		SiftMatching myMatcher;		//nMatchIndex == 2
//		BriskMatching myMatcher;	//nMatchIndex == 3
//		FreakMatching myMatcher;	//nMatchIndex == 4
		OrbMatching myMatcher;	//nMatchIndex == 5

		int nMatchIndex = 5;

		logFile.open("/mnt/sdcard/SkyEyeImage/IM_TimePerm_SizeInRange.csv", ofstream::out | ofstream::app);

		if (nMatchIndex == 1) {
			logFile<<"----------SURF----------"<<"\n";
		} else if (nMatchIndex == 2) {
			logFile<<"----------SIFT----------"<<"\n";
		} else if (nMatchIndex == 3) {
			logFile<<"----------SURF/Brisk----------"<<"\n";
		} else if (nMatchIndex == 4) {
			logFile<<"----------SURF/Freak----------"<<"\n";
		} else if (nMatchIndex == 5) {
			logFile<<"----------ORB----------"<<"\n";
		}

		for(int i = 1; i<=406; i++) {
//			baseImageFile = sFilePathPre + itoa(distArr[i]) + "feet.jpg";
//			baseImageFile<<sFilePathPre<<distArr[i]<<"feet.jpg";
			memset(strFilenameBase, 0x00, 100);
			sprintf(strFilenameBase, "%s%d%s", "/mnt/sdcard/SkyEyeImage/IM_MeasureTime_SizeInRange/query_", i, ".jpg");
			baseImage = imread(strFilenameBase, CV_LOAD_IMAGE_GRAYSCALE);

			for(int j = i+1; j<=406; j++) {
//				receivedImageFile = sFilePathPre + distArr[j] + "feet.jpg";
//				receivedImageFile<<sFilePathPre<<distArr[j]<<"feet.jpg";
				memset(strFilenameRecv, 0x00, 100);
				sprintf(strFilenameRecv, "%s%d%s","/mnt/sdcard/SkyEyeImage/IM_MeasureTime_SizeInRange/query_", j, ".jpg");

				receivedImage = imread(strFilenameRecv, CV_LOAD_IMAGE_GRAYSCALE);

			//	dTotalDuration = 0.0;
			//	dAvgDuration = 0.0;
			//	nMatchCount = 0;
			//	nAvgMatchCount = 0;

		//		for (int k = 0; k < 10; k++) {

					processTime = 0.0;
					duration = 0.0;

					duration = static_cast<double>(getTickCount());

				//	myMatches = myMatcher.DoMatching(baseImage, receivedImage);
				//	myMatches = myMatcher.DoMatching(baseImage, baseImage);
					myMatches = myMatcher.DoMatching(baseImage, receivedImage, processTime);

					duration = static_cast<double>(getTickCount())-duration;
					duration = duration * 1.0/getTickFrequency(); // the elapsed time in s

					duration =  duration*1000.0;
					//nMatchCount = nMatchCount + myMatches.size();
					//dTotalDuration = dTotalDuration + duration;
		//		}

				//dAvgDuration = dTotalDuration/10.0;
				//nAvgMatchCount = nMatchCount/10;

//				logFile<<distArr[i]<<","<<distArr[j]<<","<<nAvgMatchCount<<","<<dAvgDuration<<"\n";
				logFile<<i<<","<<j<<","<<myMatches.size()<<","<<duration<<"\n";

			}
		}


		logFile.close();

		return;

//		double fDistance = 0.0;
//		double fDistance20 = 0.0;
//		for(int i = 0; i<myMatches.size(); i++) {
//			fDistance = fDistance + myMatches.at(i).distance;
//			if (i<20) {
//				fDistance20 = fDistance20 + myMatches.at(i).distance;
//			}
//		}

//		int nCount;
//		nCount = myMatches.size();
//		return nCount;
	}


	double detect_vehcile()
	{
		int nCount1 = 0;
		int nCount2 = 0;
		int nCount3better = 0;
		int nMaxNum = 0;
		int nDetectedCount;
		Mat originalFrame;
		Mat resizedFrame;
		Mat captureFrame; //Resized
		Mat grayscaleFrame;
		Mat roiFrame;
		char strNegFile[150];
		static int nFileIdx = 0;
		String vehicle_cascade_name = "/mnt/sdcard/SkyEyeImage/cascade.xml";
		ofstream logFile;

///		int nFrameCount = 2252;  //Total being detected Frame Number
		int nFrameCount = 2000;  //Total being detected Frame Number

		//string video_file = "/mnt/sdcard/SkyEyeImage/sample2.mp4";
		char strFilename[120];

		double fResizeFactor = 0.6;

		int nCount = 0;
		int nTotalCount = 0;
		double dTotalTime = 0.0;
		double dAvgTime = 0.0;

		double durationframe; //Duration for processing each frame
		double duration; //Duration for processing the total video file

		logFile.open("/mnt/sdcard/SkyEyeImage/VehicleDetectionTime_60.csv", ofstream::out | ofstream::app);

		//create the cascade classifier object used for the face detection
		CascadeClassifier vehicle_cascade;

		//use the haarcascade_frontalface_alt.xml library
//		vehicle_cascade.load(vehicle_cascade_name);
		vehicle_cascade.load("/mnt/sdcard/SkyEyeImage/cascade.xml");

		//setup video capture device and link it to the first capture device
//		VideoCapture captureDevice(video_file);
		///VideoCapture captureDevice("/mnt/sdcard/SkyEyeImage/sample2.avi");

		//Don't Write VideoWriter outputVideo;

		//captureDevice.open(0);    //Open default camera

		//Don't Write int codec = static_cast<int>(captureDevice.get(CV_CAP_PROP_FOURCC));
		///double framerate = captureDevice.get(CV_CAP_PROP_FPS);
		//int delay = (1000/framerate)/2;

		//Get total frames in the video
		///int nFrameCount = captureDevice.get(CV_CAP_PROP_FRAME_COUNT);


		durationframe = static_cast<double>(getTickCount());

		Mat originalFrame0;

//		int nCheckIdx = 0;

		//create a loop to capture and find faces
	//	while(true) {
		for (int i = 1; i<=nFrameCount; i++) {
			//capture a new image frame
			memset(strFilename, 0x00, 120);
			sprintf(strFilename, "%s%d%s", "/mnt/sdcard/SkyEyeImage/VideoFrame/image_", i, ".jpg");
//			duration = static_cast<double>(getTickCount());

			//create a vector array to store the face found
			std::vector<Rect> vehicles;

			originalFrame = imread(strFilename);

			//cv::resize(originalFrame, resizedImg, cv::Size(25, 25));


//			if (!captureDevice.read(originalFrame)) break;

//			nCheckIdx = ( nCheckIdx + 1) % 3;
//			if (nCheckIdx != 0) continue;

			//originalFrame = rotateImage(originalFrame, 180);

			resize(originalFrame, captureFrame, Size(originalFrame.cols*fResizeFactor, originalFrame.rows*fResizeFactor));
			//Define ROI
			cv::Rect rect(0, captureFrame.rows/3, captureFrame.cols, captureFrame.rows*2/3);
			//cv::Rect rect(captureFrame.cols/3, captureFrame.rows/3, captureFrame.cols*2/3, captureFrame.rows*2/3);

			duration = static_cast<double>(getTickCount());

			roiFrame = captureFrame(rect);
			//convert captured image to gray scale and equalize
			///cvtColor(captureFrame, grayscaleFrame, CV_BGR2GRAY);
			cvtColor(roiFrame, grayscaleFrame, CV_BGR2GRAY);

			equalizeHist(grayscaleFrame, grayscaleFrame);


			//find faces and store them in the vector array
			vehicle_cascade.detectMultiScale( grayscaleFrame, vehicles, 1.1, 3, 0|CV_HAAR_SCALE_IMAGE, Size(40,40));

			nDetectedCount = vehicles.size();

			//draw a rectangle for all found vehicles in the vector array on the original image
			for(int i = 0; i < nDetectedCount; i++) {
				//cout<<"Width="<<vehicles[i].width<<endl;;
		//		Point pt1(vehicles[i].x + vehicles[i].width, vehicles[i].y + vehicles[i].height);
		//		Point pt2(vehicles[i].x, vehicles[i].y);
	#if 1
				//Write into file
				Rect rect(vehicles[i].x, vehicles[i].y, vehicles[i].width, vehicles[i].height);
				memset(strNegFile, 0x00, 150);
				nFileIdx = nFileIdx +1;
			//	sprintf(strNegFile, "%s\\SelfSample%04d.jpg", saveFile.c_str(), nFileIdx);
				//cout<<strNegFile<<endl;
				//imwrite(strNegFile, roiFrame(rect));
	#endif
	//			rectangle(roiFrame, pt1, pt2, cvScalar(0, 255, 0, 0), 2, 8, 0);

			}

			if (nDetectedCount > 0) {
				//nTotalCount = nTotalCount + nDetectedCount;
				nCount = nCount + 1;  //Frame count
				duration = static_cast<double>(getTickCount())-duration;
				duration = duration*1000.0/getTickFrequency(); // the elapsed time in ms
				dTotalTime = dTotalTime + duration;
				logFile<<nDetectedCount<<","<<duration<<"\n";

			}

			//if ( waitKey(delay) >= 0 ) break;
			//waitKey(5);
		}

		//captureDevice.release();
		//Don't Write outputVideo.release();

		logFile.close();

		if (nCount > 0)
			dAvgTime = dTotalTime/nCount;

		return dAvgTime;

	}

#endif


  // JNIEXPORT jintArray JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ImgFun(JNIEnv* env, jobject obj, jintArray buf, int w, int h);
    JNIEXPORT jdouble JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ImgFun(JNIEnv* env, jobject obj, jintArray buf, int w, int h);

    JNIEXPORT jdouble JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ImgFun(JNIEnv* env, jobject obj, jintArray buf, int w, int h)
    {

//    	double nMatched = test_ImageMatch();

    	//test_ImageMatch();

    	test_compareAlgorithm();
    	//int nMatched = 3;
    	return 0;
/*
		jint *cbuf;
		cbuf = env->GetIntArrayElements(buf, false);
		if(cbuf == NULL)
		{
			return 0;
		}

		Mat myimg(h, w, CV_8UC4, (unsigned char*)cbuf);
		for(int j=0;j<myimg.rows/2;j++)
		{
			myimg.row(j).setTo(Scalar(0,0,0,0));
		}

		int size=w * h;
		jintArray result = env->NewIntArray(size);
		env->SetIntArrayRegion(result, 0, size, cbuf);
		env->ReleaseIntArrayElements(buf, cbuf, 0);
		return result;
*/
    }

    JNIEXPORT jdouble JNICALL Java_com_testopencv_haveimgfun_LibImgFun_VehicleDetection(JNIEnv* env, jobject obj);

    JNIEXPORT jdouble JNICALL Java_com_testopencv_haveimgfun_LibImgFun_VehicleDetection(JNIEnv* env, jobject obj)
    {
    	double dAvgTime = 0.0;

    	dAvgTime = detect_vehcile();

    	return dAvgTime;
    }



    JNIEXPORT jintArray JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ConvertToGray(JNIEnv* env, jobject obj, jintArray buf, int w, int h);

#if 0
    JNIEXPORT jintArray JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ConvertToGray(JNIEnv* env, jobject obj, jintArray buf, int w, int h)
    {

	   jint *cbuf;
		cbuf = env->GetIntArrayElements(buf, false);
		if(cbuf == NULL){
			return 0;
		}

		Mat imgData(h, w, CV_8UC4, (unsigned char*)cbuf);

		uchar* ptr = imgData.ptr(0);
		for(int i = 0; i < w*h; i ++){
			//计算公式：Y(亮度) = 0.299*R + 0.587*G + 0.114*B
			//对于一个int四字节，其彩色值存储方式为：BGRA
			int grayScale = (int)(ptr[4*i+2]*0.299 + ptr[4*i+1]*0.587 + ptr[4*i+0]*0.114);
			ptr[4*i+1] = grayScale;
			ptr[4*i+2] = grayScale;
			ptr[4*i+0] = grayScale;
		}

		int size=w * h;
		jintArray result = env->NewIntArray(size);
		env->SetIntArrayRegion(result, 0, size, cbuf);
		env->ReleaseIntArrayElements(buf, cbuf, 0);
		return result;
    }

#else

    JNIEXPORT jintArray JNICALL Java_com_testopencv_haveimgfun_LibImgFun_ConvertToGray(JNIEnv* env, jobject obj, jintArray buf, int w, int h)
    {
		jint *cbuf;
		cbuf = env->GetIntArrayElements(buf, false);
		if(cbuf == NULL)
		{
			return 0;
		}

		Mat imgData(h, w, CV_8UC4, (unsigned char*)cbuf);

		int size=w * h;

		jintArray arr1 = env->NewIntArray(size);
		if(arr1 == NULL)
		{
			return 0;
		}

		jint* cbufGray;
		cbufGray=env->GetIntArrayElements(arr1, false);
		if(cbufGray == NULL)
		{
			return 0;
		}

		//Mat grayImage;
		Mat grayImage(h, w, CV_8UC1, (uchar*) cbufGray);
		//Mat grayImage(h, w, CV_8UC1);
		//Mat grayImage;
		//Mat grayTmp(h,w, CV_8UC1);
		//Mat myimg(h, w, CV_8UC4, cbuf);
		Mat tmpMat(h,w, CV_8UC3);

		cvtColor(imgData, tmpMat, CV_BGRA2BGR);
		cvtColor(tmpMat, grayImage, CV_BGR2GRAY);

		//uchar* cbufGray = grayImage.ptr(0);
/*
		int size=w * h;
		jintArray result = env->NewIntArray(size);
		//env->SetIntArrayRegion(result, 0, size, cbuf);
		env->SetIntArrayRegion(result, 0, size, cbufGray);
		env->ReleaseIntArrayElements(buf, cbufGray, 0);
		env->ReleaseIntArrayElements(buf, cbuf, 0);

		return result;

*/
//		env->ReleaseIntArrayElements(graybuf, cbufGray, 0);



		//jintArray result = env->NewIntArray(size);
		//env->SetIntArrayRegion(result, 0, size, cbufGray);

		//int size=w * h;
		jintArray result = env->NewIntArray(size);
		env->SetIntArrayRegion(result, 0, size, cbufGray);
		env->ReleaseIntArrayElements(buf, cbuf, 0);
//		env->ReleaseIntArrayElements(retBuf, cbufGray, 0);

		//return;

		return result;

    }
#endif


}
