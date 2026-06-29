#pragma once
#include <opencv2/opencv.hpp>

class IFeedManager
{
public:
	virtual ~IFeedManager() = default;

	virtual bool Initialize() = 0;
	virtual bool ReadNextFrame(cv::Mat& frame) = 0;
	virtual int GetFrameWidth() = 0;
	virtual int GetFrameHeight() = 0;
	virtual bool IsFeedOpen() = 0;
	virtual void Run() = 0;
};

