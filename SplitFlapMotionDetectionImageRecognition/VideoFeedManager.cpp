#include "VideoFeedManager.h"



VideoFeedManager::VideoFeedManager(const std::string& filePath)
	: videoFilePath(filePath)
{ }

VideoFeedManager::~VideoFeedManager()
{
	videoCapture.release();
	cv::destroyAllWindows();
}

bool VideoFeedManager::Initialize()
{
	videoCapture.open(videoFilePath);
	if (!videoCapture.isOpened())
	{
		std::cerr << "Error: Could not open video file: " << videoFilePath << std::endl;
		return false;
	}

	return true;
}

bool VideoFeedManager::ReadNextFrame(cv::Mat& frame)
{
	videoCapture >> frame;
	
	if (frame.empty())
	{
		LoopVideo(frame);
	}

	return !frame.empty();
}

int VideoFeedManager::GetFrameWidth()
{
	return static_cast<int>(videoCapture.get(cv::CAP_PROP_FRAME_WIDTH));
}

int VideoFeedManager::GetFrameHeight()
{
	return static_cast<int>(videoCapture.get(cv::CAP_PROP_FRAME_HEIGHT));
}

bool VideoFeedManager::IsFeedOpen()
{
	return videoCapture.isOpened();
}

void VideoFeedManager::Run()
{
	cv::Mat frame;
	bool isRunning = true;

	while (isRunning)
	{
		if (!ReadNextFrame(frame))
		{
			std::cerr << "Error: Could not read frame from video!" << std::endl;
			break;
		}

		cv::imshow("Video Feed", frame);


		char key = (char)cv::waitKey(16);
		if (key == 27) // ESC key
		{
			isRunning = false;
		}
	}
}

void VideoFeedManager::LoopVideo(cv::Mat& frame)
{
	std::cout << "Video ended. Rewinding..." << std::endl;
	videoCapture.set(cv::CAP_PROP_POS_FRAMES, 0);
	videoCapture >> frame;
}
