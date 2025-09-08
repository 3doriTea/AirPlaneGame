#pragma once
#include <cstdint>
namespace mtgb
{
	enum struct Axis : uint8_t
	{
		X,
		Y,
		Z
	};

	enum struct PadAxisCode : uint8_t
	{
		LeftTrigger,
		RightTrigger,
		/*LeftStick,
		RightStick*/
	};

	enum struct FlightStickAxisCode : uint8_t
	{
		Slider
	};
}