#pragma once
#include <opencv2/opencv.hpp>

enum class MotionState
{
	NotRotating,
	Rotating
};

enum class MessageType
{
	INFO,
	ERRORS
};

struct ImageTarget
{
	std::string imagePath;
	cv::Mat image;
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;
};