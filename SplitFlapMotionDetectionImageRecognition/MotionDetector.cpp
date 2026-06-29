#include "MotionDetector.h"
#include <opencv2/core/utils/logger.hpp>

MotionDetector::MotionDetector(IFeedManager& feedManager)
	:feedManager(feedManager), currentState(MotionState::NotRotating), motionPixels(0)
{
}

void MotionDetector::Initialize(int motionThreshold, int debounceMotionThreshold, int debounceStillThreshold, int mog2History, int mog2Threshold, cv::Rect roi)
{
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_SILENT);

	this->motionThreshold = motionThreshold;
	this->debounceMotionThreshold = debounceMotionThreshold;
	this->debounceStillThreshold = debounceStillThreshold;
	this->mog2History = mog2History;
	this->mog2Threshold = mog2Threshold;
	this->roi = roi;
	motionFrameCount = 0;
	stillFramesCount = 0;
	textColor = cv::Scalar(0, 255, 0);

	// --- MOG2 (MIXTURE OF GAUSSIANS ---
	// Instead of comparing the current frame to just one previous frame, MOG2 analyzes the history of every single pixel over time 
	// to determine if a change is actual motion of just random noise.
	// The parameters for MOG2 are:
	// 1.history -> how many past frames the algorithm analyzes to build and mantain its background model. 
	//				if the video runs at 60fps, 120 means the algorithm evaluates roughly the last 2 seconds of video
	// 2.varThreshold -> determines how radically a pixel current color/brightness must depart from its historical average to be considered as motion
	// 3.detectShadows -> if true, the algorithm will try to detect shadows and mark them as a separate class of motion.
	backgroundSubtractor = cv::createBackgroundSubtractorMOG2(120, 32, false);

	ValidateROI();
}

void MotionDetector::ProcessFrame(cv::Mat& frame)
{
	//Crop the frame to the region of interest
	cv::Mat roi_frame = frame(roi);

	//Feed the cropped frame to the MOG2 background subtractor to get a binary mask of motion areas
	backgroundSubtractor->apply(roi_frame, thresholdMask, 0.005);

	//Count how many pixels in the mask are white (255)
	motionPixels = cv::countNonZero(thresholdMask);

	//MOTION DETECTED
	//If the number of white pixels exceeds the motionThreshold value, motion is detected
	if (motionPixels > motionThreshold)
	{
		//DEBOUNCE LOGIC
		//Increments the motion frame counter since motion was detected
		motionFrameCount++;
		//Reset the still frame counter since motion was detected
		stillFramesCount = 0;

		//If motion has been detected for a number of consecutive frames > debounceMotionThreshold, the state is set to "Rotating"
		if (motionFrameCount >= debounceMotionThreshold)
		{
			currentState = MotionState::Rotating;
			//Update the UI text color
			textColor = cv::Scalar(0, 0, 255);
		}
	}
	//NO MOTION DETECTED
	else
	{
		//DEBOUNCE LOGIC
		//Increments the still frame counter since no motion was detected
		stillFramesCount++;
		//Reset the motion frame counter since no motion was detected
		motionFrameCount = 0;

		//If motion has NOT been detected for a number of consecutive frames > debounceStillThreshold, the state is set to "Not Rotating"
		if (stillFramesCount >= debounceStillThreshold)
		{
			currentState = MotionState::NotRotating;
			textColor = cv::Scalar(0, 255, 0);
		}
	}
}

void MotionDetector::ToggleDebugMaskDisplay()
{
	showMotionDebugMask = !showMotionDebugMask;
}

void MotionDetector::ValidateROI()
{
	if (feedManager.IsFeedOpen())
	{
		//Fetch the frame width and height from the active capture stream
		int frameWidth = feedManager.GetFrameWidth();
		int frameHeight = feedManager.GetFrameHeight();

		//Check if the requested ROI goes outside the bounds of the actual frame dimensions
		if (roi.x < 0 || roi.width < 0 || (roi.x + roi.width) > frameWidth ||
			roi.y < 0 || roi.height < 0 || (roi.y + roi.height) > frameHeight)
		{
			std::cout << "Warning: Requested ROI " << roi
				<< " is out of bounds for stream resolution (" << frameWidth << "x" << frameHeight
				<< "). Defaulting ROI to entire frame." << std::endl;

			//Fallback: use the full frame frame as the ROI
			this->roi = cv::Rect(0, 0, frameWidth, frameHeight);
		}
	}
}
