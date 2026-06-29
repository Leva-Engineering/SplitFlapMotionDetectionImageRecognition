#include "LiveFeedManager.h"

LiveFeedManager::LiveFeedManager(int width, int height)
	: cameraWidth(width), cameraHeight(height)
{ }

LiveFeedManager::~LiveFeedManager()
{ 
	videoCapture.release();
	cv::destroyAllWindows();
}

bool LiveFeedManager::Initialize()
{
	if (!ScanAndSelectCamera())
	{
		std::cerr << "Error: No camera selected or available!" << std::endl;
		return false;
	}

	return videoCapture.isOpened();
}

bool LiveFeedManager::ReadNextFrame(cv::Mat& frame)
{
	videoCapture >> frame;
	return !frame.empty();
}

void LiveFeedManager::Run()
{
	cv::Mat frame;
	bool isRunning = true;	
	
	while (isRunning)
	{
		if (!ReadNextFrame(frame))
		{
			std::cerr << "Error: Could not read frame from camera!" << std::endl;
			break;
		}


		cv::imshow("Live Feed", frame);
		
		char key = (char)cv::waitKey(16);
		if (key == 27) // ESC key
		{
			isRunning = false;
		}
	}
}

bool LiveFeedManager::ScanAndSelectCamera()
{
	std::vector<int> availableCameras;
	std::cout << "========================================" << std::endl;
	std::cout << " SCANNING FOR AVAILABLE VIDEO DEVICES... " << std::endl;
	std::cout << "========================================" << std::endl;

	for (int i = 0; i < 6; i++)
	{
		cv::VideoCapture foundCamera(i, cv::CAP_DSHOW);
		if (foundCamera.isOpened())
		{
			availableCameras.push_back(i);
			std::cout << "[" << availableCameras.size() - 1 << "] Camera Index " << i << std::endl;
			foundCamera.release();
		}
	}

	if (availableCameras.empty())
	{
		std::cerr << "No functional cameras detected." << std::endl;
		return false;
	}

	int selection = 0;
	while (true)
	{
		std::cout << "\nSelect camera choice (0-" << availableCameras.size() - 1 << "): ";
		std::cin >> selection;

		if (std::cin.fail() || selection < 0 || selection >= static_cast<int>(availableCameras.size()))
		{
			std::cin.clear();
			std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			std::cout << "Invalid selection. Please try again." << std::endl;
		}
		else
		{
			break;
		}
	}

	int chosenIndex = availableCameras[selection];

	videoCapture.open(chosenIndex, cv::CAP_DSHOW);
	if (!videoCapture.isOpened())
	{
		std::cerr << "Error: Could not open selected camera!" << std::endl;
		return false;
	}

	videoCapture.set(cv::CAP_PROP_BUFFERSIZE, 1);
	videoCapture.set(cv::CAP_PROP_FPS, 60);
	videoCapture.set(cv::CAP_PROP_FRAME_WIDTH, cameraWidth);
	videoCapture.set(cv::CAP_PROP_FRAME_HEIGHT, cameraHeight);

	return true;
}

int LiveFeedManager::GetFrameWidth()
{
	return cameraWidth;
}

int LiveFeedManager::GetFrameHeight()
{
	return cameraHeight;
}

bool LiveFeedManager::IsFeedOpen()
{
	return videoCapture.isOpened();
}