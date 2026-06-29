#pragma once
#include "IFeedManager.h"
#include "MotionDetector.h"
#include "ImageTracker.h"
#include "utils.h"

class App
{
private:
	bool isRunning = false;

private:
	IFeedManager& feedManager;
	MotionDetector* motionDetector;
	ImageTracker* imageTracker;

public:
	App(IFeedManager& feedManager, MotionDetector* motionDetector = nullptr, ImageTracker* imageTracker = nullptr);
	~App();

	void Run();

private:
	void HandleKeyboardInput();
};

