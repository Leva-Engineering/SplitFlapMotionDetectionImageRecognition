#include "ImageTracker.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/calib3d.hpp>

ImageTracker::ImageTracker(IFeedManager& feedManager)
	:feedManager(feedManager), isTracking(false)
{ }

ImageTracker::~ImageTracker()
{
	bruteForceMatcher.release();
	orbDetector.release();	
}

bool ImageTracker::Initialize(const std::string& referenceImagePath)
{
	if (!LoadReferenceImage(referenceImagePath, referenceImage))
	{
		std::cerr << "Error: Could not load reference image." << std::endl;
		return false;
	}

	orbDetector = cv::ORB::create();
	bruteForceMatcher = cv::BFMatcher::create(cv::NORM_HAMMING);	

	orbDetector->detectAndCompute(referenceImage, cv::noArray(), referenceKeypoints, referenceDescriptors);
	if (referenceKeypoints.empty() || referenceDescriptors.empty())
	{
		std::cerr << "Error: Could not detect keypoints or compute descriptors for the reference image." << std::endl;
		return false;
	}

	return true;
}

void ImageTracker::DetectAndMatch(cv::Mat& frame)
{
	cv::Mat grayFrame;
	cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);

	std::vector<cv::KeyPoint> frameKeypoints;
	cv::Mat frameDescriptors;
	orbDetector->detectAndCompute(grayFrame, cv::noArray(), frameKeypoints, frameDescriptors);

	bool currentDetection = false;

	if (referenceDescriptors.empty() || frameDescriptors.empty()) 
	{
		return;
	}

	std::vector<std::vector<cv::DMatch>> knnMatches;
	bruteForceMatcher->knnMatch(referenceDescriptors, frameDescriptors, knnMatches, 2);

	std::vector<cv::DMatch> goodMatches;
	for (size_t i = 0; i < knnMatches.size(); i++)
	{
		if (knnMatches[i].size() >= 2)
		{
			if (knnMatches[i][0].distance < 0.75 * knnMatches[i][1].distance)
			{
				goodMatches.push_back(knnMatches[i][0]);
			}
		}
	}

	if (goodMatches.size() >= 4)
	{
		std::vector<cv::Point2f> srcPoints;
		std::vector<cv::Point2f> dstPoints;

		for (size_t i = 0; i < goodMatches.size(); i++)
		{
			srcPoints.push_back(referenceKeypoints[goodMatches[i].queryIdx].pt);
			dstPoints.push_back(frameKeypoints[goodMatches[i].trainIdx].pt);
		}

		std::vector<uchar> inliersMask;
		cv::Mat H = cv::findHomography(srcPoints, dstPoints, cv::RANSAC, 5.0, inliersMask);

		int inliersCount = 0;
		for (size_t i = 0; i < inliersMask.size(); i++)
		{
			if (inliersMask[i])
				inliersCount++;
		}

		if (inliersCount > 12 && !H.empty())
		{
			currentDetection = true;

			// Draw the text overlay directly on the frame matrix
			cv::putText(frame, "OBJECT DETECTED!", cv::Point(30, 50),
				cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);

			std::vector<cv::Point2f> templateCorners(4);
			templateCorners[0] = cv::Point2f(0, 0);
			templateCorners[1] = cv::Point2f((float)referenceImage.cols, 0);
			templateCorners[2] = cv::Point2f((float)referenceImage.cols, (float)referenceImage.rows);
			templateCorners[3] = cv::Point2f(0, (float)referenceImage.rows);

			std::vector<cv::Point2f> sceneCorners(4);
			cv::perspectiveTransform(templateCorners, sceneCorners, H);

			// Draw the bounding box directly on the frame matrix
			cv::line(frame, sceneCorners[0], sceneCorners[1], cv::Scalar(0, 255, 0), 4);
			cv::line(frame, sceneCorners[1], sceneCorners[2], cv::Scalar(0, 255, 0), 4);
			cv::line(frame, sceneCorners[2], sceneCorners[3], cv::Scalar(0, 255, 0), 4);
			cv::line(frame, sceneCorners[3], sceneCorners[0], cv::Scalar(0, 255, 0), 4);
		}
	}

	if (currentDetection && !isTracking)
	{
		std::cout << "[STATE] Image detected!" << std::endl;
		isTracking = true;
	}
	else if (!currentDetection && isTracking)
	{
		std::cout << "[STATE] Tracking lost!" << std::endl;
		isTracking = false;
	}
}

bool ImageTracker::LoadReferenceImage(const std::string& imagePath, cv::Mat& image)
{
	image = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
	if (image.empty())
	{
		std::cerr << "Error: Could not load reference image from path: " << imagePath << std::endl;
		return false;
	}

	return true;
}


