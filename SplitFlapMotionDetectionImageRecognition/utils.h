#pragma once

#include <string>
#include "customtypes.h"

inline std::string MotionStateToStr(MotionState state)
{
	switch (state)
	{
	case MotionState::NotRotating:
		return "Not Rotating";
	case MotionState::Rotating:
		return "Rotating";
	default:
		return "Unknown";
	}
}

inline std::string GetTimestampStr()
{
	auto now = std::chrono::system_clock::now();
	std::time_t time_now = std::chrono::system_clock::to_time_t(now);

	std::tm local_time;
	localtime_s(&local_time, &time_now);

	std::stringstream ss;
	ss << std::put_time(&local_time, "%m/%d/%Y %H:%M:%S");

	return ss.str();
}