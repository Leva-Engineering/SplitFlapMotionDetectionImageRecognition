#pragma once
#include "IFeedManager.h"
#include "MotionDetector.h"

class App
{
private:
	bool isRunning = false;

private:
	IFeedManager& feedManager;
	MotionDetector& motionDetector;

public:
	App(IFeedManager& feedManager, MotionDetector& motionDetector);
	~App();

	void Run();

private:
	void HandleKeyboardInput();
};

