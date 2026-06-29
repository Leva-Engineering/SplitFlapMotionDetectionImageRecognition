#include "App.h"
#include "LiveFeedManager.h"
#include "VideoFeedManager.h"	
#include "utils.h"
#include <chrono>

App::App(IFeedManager& feedManager, MotionDetector* motionDetector, ImageTracker* imageTracker)
	: feedManager(feedManager), motionDetector(motionDetector), imageTracker(imageTracker)
{ }

App::~App()
{
	cv::destroyAllWindows();
}

void App::Run()
{
	cv::Mat frame;
	isRunning = true;	
	const std::string mainWindowName = "Split Flap Monitor System";
	const std::string debugWindowName = "Debug Mask";

	while (isRunning)
	{
		auto frameStart = std::chrono::steady_clock::now();

		if (!feedManager.ReadNextFrame(frame))
		{
			std::cerr << "Error: Could not read frame from feed." << std::endl;
			break;
		}

		if (motionDetector)
			motionDetector->ProcessFrame(frame);

		if (imageTracker)
			imageTracker->DetectAndMatch(frame);

		if (motionDetector)
		{
			cv::putText(frame, "Status: " + MotionStateToStr(motionDetector->GetCurrentState()), cv::Point(20, 40),
				cv::FONT_HERSHEY_SIMPLEX, 1.0, motionDetector->GetTextColor(), 2);

			cv::putText(frame, "Motion Pixels: " + std::to_string(motionDetector->GetMotionPixels()), cv::Point(20, 80),
				cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

			if (motionDetector->ShouldShowDebugMask())
			{
				cv::imshow(debugWindowName, motionDetector->GetThresholdMask());
			}
			else
			{
				double windowProperty = cv::getWindowProperty(debugWindowName, cv::WND_PROP_VISIBLE);
				if (windowProperty > 0)
				{
					cv::destroyWindow(debugWindowName);
				}
			}
		}

		cv::putText(frame, "FPS: " + std::to_string(static_cast<int>(fps)), cv::Point(20, 110),
			cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 1);

		cv::imshow("Split Flap Monitor (MOG2)", frame);
		
		HandleKeyboardInput();
		
		Force60FPS(frameStart);
	}
}

void App::Force60FPS(const std::chrono::steady_clock::time_point& frameStart)
{
	// 1. Calculate and update rolling FPS display variable once per second
	frameCount++;
	auto currentTime = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = currentTime - lastTime;
	if (elapsed.count() >= 1.0)
	{
		fps = frameCount / elapsed.count();
		frameCount = 0;
		lastTime = currentTime;
	}

	// 2. Define the exact duration 1 full frame should take at 60 FPS (~16.666 ms)
	const std::chrono::duration<double> frameDuration(1.0 / 60.0);

	// 3. Spin lock until the total duration since frameStart matches our frame time target
	while (std::chrono::steady_clock::now() - frameStart < frameDuration)
	{
		// Relinquish remaining slice of CPU scheduling quantum to prevent 100% core usage spikes
		std::this_thread::yield();
	}
}

void App::HandleKeyboardInput()
{
	char key = (char)cv::waitKey(1);
	if (key == 27) // ESC key
	{
		isRunning = false;
	}
	else if (key == 'd' || key == 'D')
	{
		motionDetector->ToggleDebugMaskDisplay();
	}
}


