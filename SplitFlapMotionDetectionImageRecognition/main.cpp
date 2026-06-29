#include <opencv2/opencv.hpp>
#include <iostream>
#include "LiveFeedManager.h"
#include "VideoFeedManager.h"

int main() 
{
	/*LiveFeedManager liveFeedManager(1280, 720);
	if (!liveFeedManager.Initialize())
	{
		std::cerr << "Failed to initialize the live feed manager." << std::endl;
		return -1;
	}

	liveFeedManager.Run();*/
	
	VideoFeedManager videoFeedManager("split_flap2.mp4");
	if (!videoFeedManager.Initialize())
	{
		std::cerr << "Failed to initialize the video feed manager." << std::endl;
		return -1;
	}

	videoFeedManager.Run();	
    return 0;
}