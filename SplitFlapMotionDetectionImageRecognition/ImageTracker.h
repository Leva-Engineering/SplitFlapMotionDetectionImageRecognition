#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <atomic>
#include "IFeedManager.h"

class ImageTracker
{
private:
	IFeedManager& feedManager;
	cv::Mat referenceImage;
	//ORB (Oriented FAST and Rotated BRIEF) is a fast and efficient algorithm to find key points (corners/edges) in an image
	cv::Ptr<cv::ORB> orbDetector;
	//Keypoints are the coordinates of unique visual features found in the target image, they are used to match the target image with the current frame
	std::vector<cv::KeyPoint> referenceKeypoints;
	//They describe what the keypoints look like so they can be recognized in different lighting or angles
	cv::Mat referenceDescriptors;
	//Tries to match the descriptors from the reference image with the descriptors from the current frame
	cv::Ptr<cv::BFMatcher> bruteForceMatcher;
	std::atomic<bool> isTracking;

public:
	ImageTracker(IFeedManager& feedManager);
	~ImageTracker();

	bool Initialize(const std::string& referenceImagePath);
	void DetectAndMatch(cv::Mat& frame);

	bool IsTracking() const { return isTracking.load(); }

private:
	bool LoadReferenceImage(const std::string& imagePath, cv::Mat& image);
};