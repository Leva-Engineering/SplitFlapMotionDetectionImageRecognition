#include "App.h"
#include "LiveFeedManager.h"
#include "VideoFeedManager.h"	
#include "utils.h"

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
		cv::imshow("Split Flap Monitor (MOG2)", frame);
		
		HandleKeyboardInput();
	}
}

void App::HandleKeyboardInput()
{
	char key = (char)cv::waitKey(16);
	if (key == 27) // ESC key
	{
		isRunning = false;
	}
	else if (key == 'd' || key == 'D')
	{
		motionDetector->ToggleDebugMaskDisplay();
	}
}


