#pragma once
#include "IFeedManager.h"
#include "MotionDetector.h"
#include "ImageTracker.h"
#include "utils.h"

class App
{
private:
	bool isRunning = false;
	IFeedManager& feedManager;
	MotionDetector* motionDetector;
	ImageTracker* imageTracker;

	std::chrono::steady_clock::time_point lastTime = std::chrono::steady_clock::now();
	int frameCount = 0;
	double fps = 0.0;

public:
	App(IFeedManager& feedManager, MotionDetector* motionDetector = nullptr, ImageTracker* imageTracker = nullptr);
	~App();

	void Run();

private:
	void Force60FPS(const std::chrono::steady_clock::time_point& frameStart);
	void HandleKeyboardInput();
};

