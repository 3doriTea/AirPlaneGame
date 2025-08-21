#include "InputConfig.h"
#include <algorithm>
void mtgb::InputConfig::SetRange(LONG _range)
{
	xRange = _range;
	yRange = _range;
	zRange = _range;
}

void mtgb::InputConfig::SetRange(LONG _range, Axis _axis)
{
	switch (_axis)
	{
	case Axis::X:
		xRange = _range;
		break;
	case Axis::Y:
		yRange = _range;
		break;
	case Axis::Z:
		zRange = _range;
		break;
	}
}

void mtgb::InputConfig::SetDeadZone(float _deadZone)
{
	deadZone = _deadZone;
}

float mtgb::InputConfig::ApplyDeadZone(const float value) const
{
	if (std::abs(value) < deadZone)
	{
		return 0.0f;
	}
	//0`1‚Ì”ÍˆÍ‚É’¼‚·
	float sign = (value > 0) ? 1.0f : -1.0f;
	return sign * (std::abs(value) - deadZone) / (1.0f - deadZone);
}