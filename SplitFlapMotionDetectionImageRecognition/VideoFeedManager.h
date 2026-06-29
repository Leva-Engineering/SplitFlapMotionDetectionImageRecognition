#pragma once
#include "IFeedManager.h"	
#include <iostream>
#include <string>

class VideoFeedManager : public IFeedManager
{
private:
	cv::VideoCapture videoCapture;
	std::string videoFilePath;

public:
	VideoFeedManager(const std::string& filePath);
	~VideoFeedManager() override;

	bool Initialize() override;
	bool ReadNextFrame(cv::Mat& frame) override;
	int GetFrameWidth() override;
	int GetFrameHeight() override;
	bool IsFeedOpen() override;
	void Run() override;

private:
	void LoopVideo(cv::Mat& frame);
};

