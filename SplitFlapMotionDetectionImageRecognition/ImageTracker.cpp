#include "ImageTracker.h"
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

ImageTracker::ImageTracker(IFeedManager& feedManager)
	: feedManager(feedManager), isTracking(false)
{
}

ImageTracker::~ImageTracker()
{
	bruteForceMatcher.release();
	orbDetector.release();
}

bool ImageTracker::Initialize(const std::string& referenceImagePath)
{
	//Loads the reference image in grayscale
	if (!LoadReferenceImage(referenceImagePath, referenceImage)) 
		return false;

	//Setup an instance of the ORB detector
	orbDetector = cv::ORB::create(); 
	//Setup an instance of the Brute Force Matcher with Hamming distance
	bruteForceMatcher = cv::BFMatcher::create(cv::NORM_HAMMING);

	//Detects keypoints and computes descriptors for the reference image
	orbDetector->detectAndCompute(referenceImage, cv::noArray(), referenceKeypoints, referenceDescriptors);

	return !referenceKeypoints.empty() && !referenceDescriptors.empty();
}

/*
* Runs continously on a dedicated background thread and determines if a the reference image is detected in the current frame
*/
void ImageTracker::DetectAndMatch(cv::Mat& frame)
{
	//Converts the the incoming color frame from the feed to a grasycale frame
	cv::Mat grayFrame;
	cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

	//Current frame keypoints
	std::vector<cv::KeyPoint> frameKeypoints;
	//Current frame keyponts descriptors
	cv::Mat frameDescriptors;
	//Detects keypoints and computes descriptors for the current grayscale frame
	orbDetector->detectAndCompute(grayFrame, cv::noArray(), frameKeypoints, frameDescriptors);

	bool currentDetection = false;

	if (referenceDescriptors.empty() || frameDescriptors.empty()) 
		return;

	std::vector<std::vector<cv::DMatch>> knnMatches;
	//Compares every single feature descriptor from your reference image against the features found in the new frame.
	bruteForceMatcher->knnMatch(referenceDescriptors, frameDescriptors, knnMatches, 2);

	//1.K-Nearest Neighbors (KNN) Feature Matching
	
	//Iterates through all match pairs to weed out poor or ambiguous data
	std::vector<cv::DMatch> goodMatches;
	for (size_t i = 0; i < knnMatches.size(); i++)
	{
		if (knnMatches[i].size() >= 2 && knnMatches[i][0].distance < 0.75 * knnMatches[i][1].distance)
		{
			goodMatches.push_back(knnMatches[i][0]);
		}
	}

	//2.Homography and RANSAC Outlier Removal 
	//You need a mathematical minimum of 4 points to map a 2D perspective transformation plane between two spaces
	if (goodMatches.size() >= 4)
	{
		std::vector<cv::Point2f> srcPoints, dstPoints;
		for (size_t i = 0; i < goodMatches.size(); i++)
		{
			srcPoints.push_back(referenceKeypoints[goodMatches[i].queryIdx].pt);
			dstPoints.push_back(frameKeypoints[goodMatches[i].trainIdx].pt);
		}

		std::vector<uchar> inliersMask;
		cv::Mat H = cv::findHomography(srcPoints, dstPoints, cv::RANSAC, 5.0, inliersMask);


		//3.Object Detection Verification & State Management
		int inliersCount = 0;
		for (size_t i = 0; i < inliersMask.size(); i++)
		{
			if (inliersMask[i]) inliersCount++;
		}

		if (inliersCount > 12 && !H.empty()) 
			currentDetection = true;
	}

	//Trigger state change just once when the image is detected or lost
	if (currentDetection && !isTracking.load())
	{
		std::cout << "[STATE] Image detected!" << std::endl;
		isTracking.store(true);
	}
	else if (!currentDetection && isTracking.load())
	{
		std::cout << "[STATE] Tracking lost!" << std::endl;
		isTracking.store(false);
	}
}

bool ImageTracker::LoadReferenceImage(const std::string& imagePath, cv::Mat& image)
{
	image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
	return !image.empty();
}