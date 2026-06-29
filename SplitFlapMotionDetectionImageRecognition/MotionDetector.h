#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>
#include "customtypes.h"
#include "IFeedManager.h"

class MotionDetector
{
private:
	IFeedManager& feedManager;
	MotionState currentState;

	//Any frame with more changing pixels than this threshold value is treated as active motion
	int motionThreshold;
	//The number of consecutive motion frames required to trigger a "Rotating" state
	int debounceMotionThreshold;
	//The number of consecutive still frames required to trigger a "Not Rotating" state
	int debounceStillThreshold;
	//Tracks how many consecutive frames have shown significant motion
	int motionFrameCount;
	//Tracks how many consecutive frames have shown little to no motion
	int stillFramesCount;
	//How many past frames the algorithm analyzes to build and mantain its background model.
	int mog2History;
	//How radically a pixel current color/brightness must depart from its historical average to be considered as motion
	int mog2Threshold;
	bool showMotionDebugMask = false;

	//UI text color
	cv::Scalar textColor;
	//Region of interest for motion detection
	cv::Rect roi;
	//MOG2 background subtractor for motion detection
	cv::Ptr<cv::BackgroundSubtractorMOG2> backgroundSubtractor;
	//Black and white binary matrix where white pixels represent areas of motion and black pixels represent areas of no motion 
	cv::Mat thresholdMask;
	//Count how many pixels in the mask are white (255)
	int motionPixels;

public:
	MotionDetector(IFeedManager& feedManager);
	~MotionDetector() = default;

	void Initialize(int motionThreshold = 6000, int debounceMotionThreshold = 5, int debounceStillThreshold = 15, int mog2History = 120, int mog2Threshold = 32, cv::Rect roi = cv::Rect(473, 112, 507, 443));
	void ProcessFrame(cv::Mat& frame);
	void ToggleDebugMaskDisplay();

	MotionState GetCurrentState() const { return currentState; }
	cv::Scalar GetTextColor() const { return textColor; }
	int GetMotionPixels() const { return motionPixels; }
	bool ShouldShowDebugMask() const { return showMotionDebugMask; }
	const cv::Mat& GetThresholdMask() const { return thresholdMask; }

private:
	void ValidateROI();
};

