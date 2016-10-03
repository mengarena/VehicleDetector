#include <cmath>

#include <iostream>
#include <fstream>

#include "SiftMatching.h"


SiftMatching::SiftMatching(void):ratio(0.75f),distance(2.0),confidence(0.98),refineF(true) //ratio=0.8, confidence=0.98
{
}


SiftMatching::~SiftMatching(void)
{
}


void SiftMatching::setRatio(float rt)
{
	ratio = rt;
}


void SiftMatching::setDistance(double dst)
{
	distance = dst;
}


void SiftMatching::setConfidence(double confd)
{
	confidence = confd;
}


//Clear matches for which NN ratio is > threshold, return the number of removed points
//(corresponding entries are cleared)
int SiftMatching::ratioTest(vector< vector< DMatch > > &matches)
{
	int removed = 0;

	for (vector< vector< DMatch > >::iterator matchIterator = matches.begin(); matchIterator != matches.end(); ++matchIterator) {
		//If 2 NN has been identified
		if (matchIterator->size() > 1) {
			//Check distance ratio
			if ((*matchIterator)[0].distance/(*matchIterator)[1].distance > ratio) {
				matchIterator->clear();	//Remove match
				removed++;
			}
		} else {	//Does not have 2 neighbours
			matchIterator->clear();	//Remove match
			removed++;
		}
	}

	return removed;
}


//Symmetrical matches, save result in symMatches 
void SiftMatching::symmetryTest(const vector< vector< DMatch > >& matches1,const vector< vector< DMatch > >& matches2, vector< DMatch >& symMatches)
{
	//For all matches image1->image2
	for(vector< vector< DMatch > >::const_iterator matchIterator1 = matches1.begin(); matchIterator1 != matches1.end(); ++matchIterator1) {
		//Ignore deleted matches
		if (matchIterator1->size() < 2) continue;

		//For all matches image2->image1
		for(vector< vector< DMatch > >::const_iterator matchIterator2 = matches2.begin(); matchIterator2 != matches2.end(); ++matchIterator2) {
			//Ignore deleted matches
			if (matchIterator2->size() < 2) continue;

			//Match symmetry test
			if ((*matchIterator1)[0].queryIdx == (*matchIterator2)[0].trainIdx &&
				(*matchIterator2)[0].queryIdx == (*matchIterator1)[0].trainIdx) {
					//Add symmetrical match
					symMatches.push_back(DMatch((*matchIterator1)[0].queryIdx, (*matchIterator1)[0].trainIdx, (*matchIterator1)[0].distance));
					break; //Next match in image1->image2
			}
		}
	}
}


void SiftMatching::ransacTest(const vector< DMatch >& matches, const vector< KeyPoint >& keypoints1, const vector< KeyPoint >& keypoints2, vector< DMatch >& outMatches)
{
	//Convert keypoints into Point2f
	vector< Point2f > points1,points2;

	for(vector< DMatch >::const_iterator it = matches.begin(); it != matches.end(); ++it) {
		//Get the position of left keypoints
		float x = keypoints1[it->queryIdx].pt.x;
		float y = keypoints1[it->queryIdx].pt.y;
		points1.push_back(Point2f(x,y));
		//Get the position of right keypoints
		x = keypoints2[it->trainIdx].pt.x;
		y = keypoints2[it->trainIdx].pt.y;
		points2.push_back(Point2f(x,y));
	}

	//Computer Fundamental matrix using RANSAC
	vector< uchar > inliers(points1.size(), 0);
	Mat fundamental = findFundamentalMat( Mat(points1), Mat(points2), inliers, CV_FM_RANSAC, distance, confidence);

	//Extract the surviving(inliers) matches
	vector<uchar>::const_iterator itIn = inliers.begin();
	vector< DMatch >::const_iterator itM = matches.begin();

	//cout<<"Find.Before......... " << inliers.size() << endl;

	//For all matches
	for (; itIn != inliers.end(); ++itIn, ++itM) {
		if (*itIn) {	//It is a valid match
			outMatches.push_back(*itM);
		}
	}

#if 0
	///E Wai
	vector<uchar>  inliersaa(keypoints1.size(), 0);
	Mat homography = findHomography( Mat(points1), Mat(points1), inliersaa, CV_RANSAC, 1);
	cout<<"Find..qqq........ " << inliersaa.size() << endl;
	vector<uchar>::const_iterator itInaa = inliersaa.begin();
	for (; itInaa != inliersaa.end(); ++itInaa) {
		cout<<"findHomography----------"<<*itInaa<<endl;
	}
#endif

#if 0
	if (refineF) {
		//The fundamental matrix will be recomputed with all accepted matches
		//Convert keypoints into Point2f for final fundamental computation
		points1.clear();
		points2.clear();

		for(vector<DMatch>::const_iterator it = outMatches.begin(); it != outMatches.end(); ++it) {
			//Get the position of left keypoints
			float x = keypoints1[it->queryIdx].pt.x;
			float y = keypoints1[it->queryIdx].pt.y;
			points1.push_back(Point2f(x,y));
			//Get the position of right keypoints
			x = keypoints2[it->trainIdx].pt.x;
			y = keypoints2[it->trainIdx].pt.y;
			points2.push_back(Point2f(x,y));
		}

		//Computer 8-point fundamental from all accepted matches
		fundamental = findFundamentalMat(Mat(points1), Mat(points2), CV_FM_8POINT);
	}

	//return fundamental
#endif

}

vector<DMatch> SiftMatching::DoMatchingSimple(Mat& image1, Mat& image2)
{
	vector< KeyPoint > keypoints1, keypoints2;
	Mat descriptors1, descriptors2;

	//SIFT method
//	SiftFeatureDetector sift(0.03, 10.);
	SiftFeatureDetector sift; //All default parameters

	SiftDescriptorExtractor siftDesc;

	//Detect SIFT feature points
	sift.detect(image1, keypoints1);
	sift.detect(image2, keypoints2);

	//Extract SIFT descriptors
	siftDesc.compute(image1, keypoints1, descriptors1);
	siftDesc.compute(image2, keypoints2, descriptors2);

#if 0
	if(descriptors1.type()!=CV_32F) {
		descriptors1.convertTo(descriptors1, CV_32F);
	}

	if(descriptors2.type()!=CV_32F) {
		descriptors2.convertTo(descriptors2, CV_32F);
	}
#endif

	//Match the two image descriptors
	//BruteForceMatcher<L2<float>> matcher; //SL2  //BruteForceMatcher is old version
	BFMatcher matcher(NORM_L2);
	//FlannBasedMatcher matcher;

	vector< DMatch > matches;

	matcher.match(descriptors1, descriptors2, matches);
	
	//Draw matching
	Mat imageMatches;
	drawMatches(image1, keypoints1, image2, keypoints2, matches, imageMatches, Scalar(0,0,255));

	namedWindow("Sift Matched");
	imshow("Sift Matched", imageMatches);


	return matches;

}


void SiftMatching::filterWithSlopeDistance(	Mat& image1, 
											Mat& image2, 
											const vector< KeyPoint >& keypoints1,
											const vector< KeyPoint >& keypoints2,
											const vector< DMatch >& inMatches,
											vector< DMatch >& finalMatches )
{
	//Two images will be merged together, so the x-axis value of the second image will be original x + width of image1
	float pi = 3.1416;
	int width1 = image1.cols;
	
	vector<float> pointsEuDist;
	vector<float> linesAngle;

	///////////////////////////////////////////////////////////////////////////////////////////
	//Calculate angle and length of lines between matched points in inMatches
	for(vector< DMatch >::const_iterator it = inMatches.begin(); it != inMatches.end(); ++it) {
		//Get the position of left keypoints
		float x1 = keypoints1[it->queryIdx].pt.x;
		float y1 = keypoints1[it->queryIdx].pt.y;

		//Get the position of right keypoints
		float x2 = keypoints2[it->trainIdx].pt.x + width1;
		float y2 = keypoints2[it->trainIdx].pt.y;

		float fEuDist = sqrt( pow((x2 - x1),2)*1.0 + pow((y2 - y1), 2) );
		float fAngle = atan( (y2 - y1)*1.0 /(x2 - x1) )*180/pi; //Get Degree

		pointsEuDist.push_back(fEuDist);
		linesAngle.push_back(fAngle);
	}


	//////////////////////////////////////////////////////////////////////////////////////
	//Filter by angles
	//Find out the most frequent angle
	//////////////////////////////////////////////////////////////////////////////////////	
	map<float, int>angleMap;
	map<float, int>::iterator itAngle;

	float fCurrentAngle, fTmpCurrentAngle;
	float maxAngle,minAngle;
	int nAngleRange = 15;
	int nFrequent = 0;

	for (int i=0; i<linesAngle.size(); i++) {
		fCurrentAngle = linesAngle.at(i);
		maxAngle = fCurrentAngle + nAngleRange;
		minAngle = fCurrentAngle - nAngleRange;

		nFrequent = 1;

		for (int j=0; j<linesAngle.size(); j++) {
			if (j!=i) {
				fTmpCurrentAngle = linesAngle.at(j);
				if ((fTmpCurrentAngle >= minAngle) && (fTmpCurrentAngle <= maxAngle)) {
					nFrequent = nFrequent + 1;
				}
			}
		}

		angleMap.insert(pair<float, int>(fCurrentAngle, nFrequent));
	}

	float mostCommonAngle = 0.0;
	int freNumAngle = 0;

	for ( itAngle = angleMap.begin(); itAngle != angleMap.end(); itAngle++ ) {
		if (itAngle->second > freNumAngle) {
			freNumAngle = itAngle->second;
			mostCommonAngle = itAngle->first;
		}
	}

	//cout<<"[Angle Frequency]: "<<freNumAngle<<endl;

	maxAngle = mostCommonAngle + nAngleRange;
	minAngle = mostCommonAngle - nAngleRange;


	//////////////////////////////////////////////////////////////////////////////////////
	//Filter by Eu distance
	//Find out the most frequent Euclidian distance
	//////////////////////////////////////////////////////////////////////////////////////
	map<float, int>euDistMap;
	map<float, int>::iterator itEuDist;

	float fCurrentEuDist, fTmpCurrentEuDist;
	float maxEuDist,minEuDist;
	float fEuDistRange = 0.20;
	int nEuDistFrequent = 0;

	for (int i=0; i<pointsEuDist.size(); i++) {
		fCurrentEuDist = pointsEuDist.at(i);
		maxEuDist = fCurrentEuDist*(1.0 + fEuDistRange);
		minEuDist = fCurrentEuDist*(1.0 - fEuDistRange);

		nEuDistFrequent = 1;

		for (int j=0; j<pointsEuDist.size(); j++) {
			if (j!=i) {
				fTmpCurrentEuDist = pointsEuDist.at(j);
				if ((fTmpCurrentEuDist >= minEuDist) && (fTmpCurrentEuDist <= maxEuDist)) {
					nEuDistFrequent = nEuDistFrequent + 1;
				}
			}
		}

		euDistMap.insert(pair<float, int>(fCurrentEuDist, nEuDistFrequent));
	}

	float mostCommonEuDist = 0.0;
	int freNumEuDist = 0;

	for ( itEuDist = euDistMap.begin(); itEuDist != euDistMap.end(); itEuDist++ ) {
		if (itEuDist->second > freNumEuDist) {
			freNumEuDist = itEuDist->second;
			mostCommonEuDist = itEuDist->first;
		}
	}

	//cout<<"[Dist Frequency]: "<<freNumEuDist<<endl;

	maxEuDist = mostCommonEuDist*(1.0 + fEuDistRange);
	minEuDist = mostCommonEuDist*(1.0 - fEuDistRange);

	///////////////////////////////////////////////////////////////////
	//Final Process
	//Only the distance and angle of matched points is in the most common group, the matched points are valid
	///////////////////////////////////////////////////////////////////
	vector< DMatch >::const_iterator itM = inMatches.begin();
	vector<float>::const_iterator itAngleV = linesAngle.begin();
	vector<float>::const_iterator itEuDistV = pointsEuDist.begin();

	//cout<<"++++++Angle: " <<minAngle << " ~ " <<maxAngle<<endl;
	//cout<<"++++++EuDist: " <<minEuDist << " ~ " <<maxEuDist<<endl;

	for (; itAngleV != linesAngle.end(); itAngleV++, itEuDistV++, itM++) {
		fCurrentAngle = *itAngleV;
		fCurrentEuDist = *itEuDistV;

		//cout<<"Current Angle = " << fCurrentAngle << ", Dist = " <<fCurrentEuDist<<endl;
		if ((fCurrentAngle >= minAngle) && (fCurrentAngle <= maxAngle) && (fCurrentEuDist >= minEuDist) && (fCurrentEuDist <= maxEuDist)) {
			finalMatches.push_back(*itM);
		}

	}

}



vector< DMatch > SiftMatching::DoMatching(Mat& image1, Mat& image2, double &time)
{
	vector< KeyPoint > keypoints1, keypoints2;
	Mat descriptors1, descriptors2;


//	cout<<"Image1 Size:" << image1.cols << "x"<<image1.rows<<endl;
//	cout<<"Image2 Size:" << image2.cols << "x"<<image2.rows<<endl;

//	cout<<"Angle==" << atan(-10.0/80)*180.0/3.1416<<endl;  //Get Degree


	//SIFT method
//	SiftFeatureDetector sift(0, 3, 0.3, 5.);
	SiftFeatureDetector sift; //All default parameters. can decide #layers, but #octaves are decided automatically according to the resolution of the image

	SiftDescriptorExtractor siftDesc;

	//The time for processing the base image (i.e. self-took image) is only an one-time investment, the time is not counted here

	//Extract SIFT feature points/feature descriptors for image 1
	sift.detect(image1, keypoints1);
	siftDesc.compute(image1, keypoints1, descriptors1);

	//////////////////////////////////
	///To Measure size
#if 0
	Size matSize;

	matSize = descriptors1.size();
	cout<<"...................................."<<endl;
	cout<<"[Size] [Cols]= " <<matSize.width <<" ,[Rows]= "<<matSize.height<<endl;
	cout<<"[Element Size]= "<<descriptors1.elemSize()<<endl;
	cout<<"[size of element channel]= " <<descriptors1.elemSize1()<<endl;
	cout<<"[Type]= "<<descriptors1.type()<<endl;
	cout<<"[Depth]= "<<descriptors1.depth()<<endl;
	cout<<"[Channel]= "<<descriptors1.channels()<<endl;
	cout<<"[Col]= "<<descriptors1.cols<<" ,[Row]= "<<descriptors1.rows<<endl;
	cout<<"[Step]= "<<descriptors1.step<<endl;
	cout<<"[Total Size] = " << descriptors1.elemSize()* matSize.width * matSize.height<<" Bytes"<<endl;
	cout<<"...................................."<<endl;

	//Write into file f:\\Fp.txt
	ofstream myfile;
	myfile.open ("F:\\Fp.txt", ios::out | ios::binary);
	//unsigned char *temp = descriptors1.data;
	myfile.write(reinterpret_cast<char*>(descriptors1.data), descriptors1.elemSize()* matSize.width * matSize.height);
	//for (int kk1 = 0; kk1< descriptors1.rows; kk1++) 
	//	for (int kk2 = 0; kk2< descriptors1.cols; kk2++)
	//		myfile<<descriptors1.at<float>(kk1, kk2);
	//myfile << (unsigned char*)(descriptors1.data);
	myfile.close();

#endif

	///////////////////////////////////
//	for (int p=0; p<keypoints1.size(); p++) {
		//cout<<"Keypoints1, octave================="<<keypoints1.at(p).octave<<endl;
//	}
	
//	double duration; //Duration for image matching
//	duration = static_cast<double>(getTickCount());

	//Extract SIFT feature points/feature descriptors for image 2
	sift.detect(image2, keypoints2);
	siftDesc.compute(image2, keypoints2, descriptors2);

#if 0
	if(descriptors1.type()!=CV_32F) {
		descriptors1.convertTo(descriptors1, CV_32F);
	}

	if(descriptors2.type()!=CV_32F) {
		descriptors2.convertTo(descriptors2, CV_32F);
	}
#endif
	//Match the two image descriptors
//	BruteForceMatcher<SL2<float>> matcher; //SL2  //BruteForceMatcher is old version
	BFMatcher matcher(NORM_L2);
	//FlannBasedMatcher matcher;
	
	vector< DMatch > matches;

	//From image1 to image2 based on k nearest neighbour (k=2)
	vector< vector< DMatch > > matches1;

	//matcher.match(descriptors1, descriptors2, matches);
	matcher.knnMatch(descriptors1, descriptors2, matches1, 2);

	//From image2 to image1 based on k nearest neighbour (k=2)
	vector< vector< DMatch > > matches2;
	matcher.knnMatch(descriptors2, descriptors1, matches2, 2);

//	cout<<"Original Matching1 = #	"<<matches1.size()<< ",		Matching2 = #	"<<matches2.size()<<endl;

	//Remove matches for which NN ratio is > threshold
	//Clean image1->image2 matches
	int removed = ratioTest(matches1);

//	cout<<"After ratio Filter, Matching1 removed = #	"<<removed <<endl;

	//Clean image2->image1 matches
	removed = ratioTest(matches2);

//	cout<<"After ratio Filter, Matching1 removed = #	"<<removed <<endl;

	//Remove non-symmetrical matches
	vector< DMatch > symMatches;
	symmetryTest(matches1, matches2, symMatches);

	if (symMatches.size() < 5) {
		symMatches.clear();
		return symMatches;
	}

//	cout<<"After symmetrical Filter, Matching = #	"<<symMatches.size() <<endl;

//	return symMatches; //////////////////////////////

	//Validate matches using RANSAC (get inliers matches)
	ransacTest(symMatches, keypoints1, keypoints2, matches);

//	cout<<"After RANSAC Filter, Matching = #	"<<matches.size() <<endl;

//	duration = static_cast<double>(getTickCount())-duration;
//	duration /= getTickFrequency(); // the elapsed time in s

#if 0 //Final filter
	vector< DMatch > finalMatches;

	double durationFinal; //Duration for image matching
	durationFinal = static_cast<double>(getTickCount());

	filterWithSlopeDistance(image1, image2, keypoints1, keypoints2, matches, finalMatches);

	durationFinal = static_cast<double>(getTickCount())-durationFinal;
	durationFinal /= getTickFrequency(); // the elapsed time in s
	durationFinal = durationFinal *1000.0; //the elapsed time in ms

	cout<<"==================Final Filter used:	" <<durationFinal<<"	ms"<<endl;

	float total = 0.0f;
	float fratio = 0.0f;
	float max1 = 0.0f;
	float max2 = 0.0f;
	for (int k = 0; k<finalMatches.size(); k++) {
//		cout<<"KeyPoints1.octave="<<keypoints1[matches.at(k).queryIdx].octave<<", KeyPoints2.octave="<<keypoints2[matches.at(k).trainIdx].octave<<endl;
		//cout<<"======Ratio="<<1.0*keypoints1[matches.at(k).queryIdx].octave/keypoints2[matches.at(k).trainIdx].octave<<endl;
		cout<<"KeyPoints1.scale="<<keypoints1[matches.at(k).queryIdx].size<<", KeyPoints2.scale="<<keypoints2[matches.at(k).trainIdx].size<<endl;
		fratio = keypoints1[finalMatches.at(k).queryIdx].size*1.2/9.0/(keypoints2[finalMatches.at(k).trainIdx].size*1.2/9.0);
		cout<<"Scale Ratio="<<fratio<<endl;
		total = total + fratio;
		if (keypoints1[finalMatches.at(k).queryIdx].size > max1) max1 = keypoints1[finalMatches.at(k).queryIdx].size;
		if (keypoints2[finalMatches.at(k).trainIdx].size > max2) max2 = keypoints2[finalMatches.at(k).trainIdx].size;
	}

	float fEuDistRatio = 0.0;

	if (finalMatches.size() >= 2) {
		float fDist1 = 0.0; //The Euclidean distance of two matching feature points in Image 1
	
		float x11, y11, x12, y12;
		x11 = keypoints1[finalMatches.at(0).queryIdx].pt.x;
		y11 = keypoints1[finalMatches.at(0).queryIdx].pt.y;

		x12 = keypoints1[finalMatches.at(2).queryIdx].pt.x;
		y12 = keypoints1[finalMatches.at(2).queryIdx].pt.y;

		fDist1 = sqrt( pow((x12 - x11),2)*1.0 + pow((y12 - y11), 2) );

		float fDist2 = 0.0; //The Euclidean distance of two matching feature points in Image 1
		float x21, y21, x22, y22;
		x21 = keypoints2[finalMatches.at(0).trainIdx].pt.x;
		y21 = keypoints2[finalMatches.at(0).trainIdx].pt.y;

		x22 = keypoints2[finalMatches.at(2).trainIdx].pt.x;
		y22 = keypoints2[finalMatches.at(2).trainIdx].pt.y;

		fDist2 = sqrt( pow((x22 - x21),2)*1.0 + pow((y22 - y21), 2) );

		cout<<"fDist1, fDist2==="<<fDist1<<","<<fDist2<<endl;


		fEuDistRatio = fDist1/fDist2;
	}

	cout<<"Euclidean Distance====================="<<fEuDistRatio<<endl;

	cout<<"[Before Final filter] =========" << matches.size() << endl;
	cout<<"[After Final filter]==========" << finalMatches.size() << endl;

	cout<<"Average Ratio ============" << total/finalMatches.size()<<endl;
	cout<<"Ratio from Max ==========="<<max1/max2<<endl;

#else

#if 0	//should be opened
	float total = 0.0f;
	float fratio = 0.0f;
	float max1 = 0.0f;
	float max2 = 0.0f;

	for (int k = 0; k<matches.size(); k++) {
//		cout<<"KeyPoints1.octave="<<keypoints1[matches.at(k).queryIdx].octave<<", KeyPoints2.octave="<<keypoints2[matches.at(k).trainIdx].octave<<endl;
		//cout<<"======Ratio="<<1.0*keypoints1[matches.at(k).queryIdx].octave/keypoints2[matches.at(k).trainIdx].octave<<endl;
		//cout<<"KeyPoints1.scale="<<keypoints1[matches.at(k).queryIdx].size<<", KeyPoints2.scale="<<keypoints2[matches.at(k).trainIdx].size<<endl;
		fratio = keypoints1[matches.at(k).queryIdx].size*1.2/9.0/(keypoints2[matches.at(k).trainIdx].size*1.2/9.0);
		//cout<<"Scale Ratio="<<fratio<<endl;
		total = total + fratio;
		if (keypoints1[matches.at(k).queryIdx].size > max1) max1 = keypoints1[matches.at(k).queryIdx].size;
		if (keypoints2[matches.at(k).trainIdx].size > max2) max2 = keypoints2[matches.at(k).trainIdx].size;
	}

	float fEuDistRatio = 0.0;

	if (matches.size() >= 2) {
		float fDist1 = 0.0; //The Euclidean distance of two matching feature points in Image 1
	
		float x11, y11, x12, y12;
		x11 = keypoints1[matches.at(0).queryIdx].pt.x;
		y11 = keypoints1[matches.at(0).queryIdx].pt.y;

		x12 = keypoints1[matches.at(2).queryIdx].pt.x;
		y12 = keypoints1[matches.at(2).queryIdx].pt.y;

		fDist1 = sqrt( pow((x12 - x11),2)*1.0 + pow((y12 - y11), 2) );

		float fDist2 = 0.0; //The Euclidean distance of two matching feature points in Image 1
		float x21, y21, x22, y22;
		x21 = keypoints2[matches.at(0).trainIdx].pt.x;
		y21 = keypoints2[matches.at(0).trainIdx].pt.y;

		x22 = keypoints2[matches.at(2).trainIdx].pt.x;
		y22 = keypoints2[matches.at(2).trainIdx].pt.y;

		fDist2 = sqrt( pow((x22 - x21),2)*1.0 + pow((y22 - y21), 2) );

		//cout<<"fDist1, fDist2==="<<fDist1<<","<<fDist2<<endl;

		float fEuDistRatio = 0.0;

		fEuDistRatio = fDist1/fDist2;
	}

#endif  //Should be opened
//	cout<<"Euclidean Distance====================="<<fEuDistRatio<<endl;

//	cout<<"[Before Final filter] =========" << matches.size() << endl;

//	cout<<"Average Ratio =====" << total/matches.size()<<endl;
//	cout<<"Ratio from Max ========="<<max1/max2<<endl;

#endif

//	cout<<"------------The processing time is:	"<<duration*1000.0<<"	ms"<<endl;

//	time = duration;

	//Draw matching
//	Mat imageMatches;
//	drawMatches(image1, keypoints1, image2, keypoints2, matches, imageMatches, Scalar(0,0,255));
//	drawMatches(image1, keypoints1, image2, keypoints2, symMatches, imageMatches, Scalar(0,0,255));

	//imwrite("f:\\tmp\\SiftMatchedBeforeFinalFilter.jpg", imageMatches);
	//namedWindow("Sift Matched");
	//imshow("Sift Matched", imageMatches);

#if 0 //Final Filter
	Mat imageMatchesFinal;
	drawMatches(image1, keypoints1, image2, keypoints2, finalMatches, imageMatchesFinal, Scalar(0,0,255));
//	drawMatches(image1, keypoints1, image2, keypoints2, symMatches, imageMatches, Scalar(0,0,255));

	//imwrite("f:\\tmp\\SiftMatchedAfterFinalFilter.jpg", imageMatchesFinal);
	namedWindow("Sift Matched Final");
	imshow("Sift Matched Final", imageMatchesFinal);

	waitKey(10000);
	return finalMatches;
#else
	//waitKey(10000);
	return matches;
#endif

	
}


