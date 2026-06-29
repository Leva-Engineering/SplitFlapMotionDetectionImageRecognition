#pragma once
#include "IFeedManager.h"

class LiveFeedManager : public IFeedManager
{
private:
	cv::VideoCapture videoCapture;
	int cameraWidth;
	int cameraHeight;

public:
	LiveFeedManager(int width = 1280, int height = 720);
	~LiveFeedManager() override;

	bool Initialize() override;
	bool ReadNextFrame(cv::Mat& frame) override;
	int GetFrameWidth() override;
	int GetFrameHeight() override;
	bool IsFeedOpen() override;

private:
	bool ScanAndSelectCamera();
};

