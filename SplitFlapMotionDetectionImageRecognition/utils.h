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