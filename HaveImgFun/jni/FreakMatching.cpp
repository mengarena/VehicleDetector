#include <cmath>

#include <iostream>
#include <fstream>

#include "FreakMatching.h"


FreakMatching::FreakMatching(void):ratio(0.75f),distance(2.0),confidence(0.98),refineF(true) //ratio=0.8, confidence=0.98
{
}


FreakMatching::~FreakMatching(void)
{
}


void FreakMatching::setRatio(float rt)
{
	ratio = rt;
}


void FreakMatching::setDistance(double dst)
{
	distance = dst;
}


void FreakMatching::setConfidence(double confd)
{
	confidence = confd;
}


//Clear matches for which NN ratio is > threshold, return the number of removed points
//(corresponding entries are cleared)
int FreakMatching::ratioTest(vector< vector< DMatch > > &matches)
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
void FreakMatching::symmetryTest(const vector< vector< DMatch > >& matches1,const vector< vector< DMatch > >& matches2, vector< DMatch >& symMatches)
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


void FreakMatching::ransacTest(const vector< DMatch >& matches, const vector< KeyPoint >& keypoints1, const vector< KeyPoint >& keypoints2, vector< DMatch >& outMatches)
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

	//For all matches
	for (; itIn != inliers.end(); ++itIn, ++itM) {
		if (*itIn) {	//It is a valid match
			outMatches.push_back(*itM);
		}
	}

}


void FreakMatching::filterWithSlopeDistance(	Mat& image1,
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


	maxEuDist = mostCommonEuDist*(1.0 + fEuDistRange);
	minEuDist = mostCommonEuDist*(1.0 - fEuDistRange);

	///////////////////////////////////////////////////////////////////
	//Final Process
	//Only the distance and angle of matched points is in the most common group, the matched points are valid
	///////////////////////////////////////////////////////////////////
	vector< DMatch >::const_iterator itM = inMatches.begin();
	vector<float>::const_iterator itAngleV = linesAngle.begin();
	vector<float>::const_iterator itEuDistV = pointsEuDist.begin();

	for (; itAngleV != linesAngle.end(); itAngleV++, itEuDistV++, itM++) {
		fCurrentAngle = *itAngleV;
		fCurrentEuDist = *itEuDistV;

		//cout<<"Current Angle = " << fCurrentAngle << ", Dist = " <<fCurrentEuDist<<endl;
		if ((fCurrentAngle >= minAngle) && (fCurrentAngle <= maxAngle) && (fCurrentEuDist >= minEuDist) && (fCurrentEuDist <= maxEuDist)) {
			finalMatches.push_back(*itM);
		}

	}

}



vector< DMatch > FreakMatching::DoMatching(Mat& image1, Mat& image2, double &time)
{
	vector< KeyPoint > keypoints1, keypoints2;
	Mat descriptors1, descriptors2;

	FREAK extractor;
	SurfFeatureDetector surf(5.); //All default parameters
//	FastFeatureDetector fast(10);
//	extractor.create("FREAK");

	surf.detect(image1, keypoints1);
	extractor.compute(image1, keypoints1, descriptors1);

	surf.detect(image2, keypoints2);
	extractor.compute(image2, keypoints2, descriptors2);

	//Match the two image descriptors
//	BruteForceMatcher<SL2<float>> matcher; //SL2  //BruteForceMatcher is old version
	BFMatcher matcher(NORM_HAMMING);
//	BruteForceMatcher<Hamming> matcher;
	//FlannBasedMatcher matcher;
//	BFMatcher matcher(NORM_L2);

	vector< DMatch > matches;

	//From image1 to image2 based on k nearest neighbour (k=2)
	vector< vector< DMatch > > matches1;

	//matcher.match(descriptors1, descriptors2, matches);
	matcher.knnMatch(descriptors1, descriptors2, matches1, 2);

	//From image2 to image1 based on k nearest neighbour (k=2)
	vector< vector< DMatch > > matches2;
	matcher.knnMatch(descriptors2, descriptors1, matches2, 2);

	//Remove matches for which NN ratio is > threshold
	//Clean image1->image2 matches
	int removed = ratioTest(matches1);

	//Clean image2->image1 matches
	removed = ratioTest(matches2);

	//Remove non-symmetrical matches
	vector< DMatch > symMatches;
	symmetryTest(matches1, matches2, symMatches);

	if (symMatches.size() < 5) {
		symMatches.clear();
		return symMatches;
	}

//	cout<<"After symmetrical Filter, Matching = #	"<<symMatches.size() <<endl;

	//Validate matches using RANSAC (get inliers matches)
	ransacTest(symMatches, keypoints1, keypoints2, matches);

//	cout<<"After RANSAC Filter, Matching = #	"<<matches.size() <<endl;

//	duration = static_cast<double>(getTickCount())-duration;
//	duration /= getTickFrequency(); // the elapsed time in s

	return matches;

}




