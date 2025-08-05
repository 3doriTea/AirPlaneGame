#include "InputData.h"

#include "Input.h"
#include "WindowContextResourceManager.h"
#include "InputResource.h"
#include "Game.h"
#include "ISystem.h"
#include "JoystickProxy.h"


const bool mtgb::InputUtil::GetKey(const KeyCode _keyCode, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return GetInput(WindowContext::First).keyStateCurrent_[Index(_keyCode)] || GetInput(WindowContext::Second).keyStateCurrent_[Index(_keyCode)];
	}

	return GetInput(_context).keyStateCurrent_[Index(_keyCode)];
}
const bool mtgb::InputUtil::GetKeyDown(const KeyCode _keyCode, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		const InputData& inputFirstWnd = GetInput(WindowContext::First);
		const InputData& inputSecondWnd = GetInput(WindowContext::Second);

		return static_cast<bool>(KeyXOR(_keyCode, inputFirstWnd.keyStateCurrent_, inputFirstWnd.keyStatePrevious_) & static_cast<int>(inputFirstWnd.keyStateCurrent_[Index(_keyCode)]))
			|| static_cast<bool>(KeyXOR(_keyCode, inputSecondWnd.keyStateCurrent_, inputSecondWnd.keyStatePrevious_) & static_cast<int>(inputSecondWnd.keyStateCurrent_[Index(_keyCode)]));
	}

	const InputData& input = GetInput(_context);
	return static_cast<bool>(KeyXOR(_keyCode, input.keyStateCurrent_, input.keyStatePrevious_) & static_cast<int>(input.keyStateCurrent_[Index(_keyCode)]));
}
const bool mtgb::InputUtil::GetKeyUp(const KeyCode _keyCode, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		const InputData& inputFirstWnd = GetInput(WindowContext::First);
		const InputData& inputSecondWnd = GetInput(WindowContext::Second);

		return static_cast<bool>(KeyXOR(_keyCode, inputFirstWnd.keyStateCurrent_, inputFirstWnd.keyStatePrevious_) & static_cast<int>(inputFirstWnd.keyStatePrevious_[Index(_keyCode)]))
			|| static_cast<bool>(KeyXOR(_keyCode, inputSecondWnd.keyStateCurrent_, inputSecondWnd.keyStatePrevious_) & static_cast<int>(inputSecondWnd.keyStatePrevious_[Index(_keyCode)]));
	}

	const InputData& input = GetInput(_context);
	return static_cast<bool>(KeyXOR(_keyCode, input.keyStateCurrent_, input.keyStatePrevious_) & input.keyStatePrevious_[Index(_keyCode)]);
}
const bool mtgb::InputUtil::GetMouse(const MouseCode _mouseCode, WindowContext _context)
{
	return false;
}
const bool mtgb::InputUtil::GetMouseDown(const MouseCode _mouseCode, WindowContext _context)
{
	return false;
}
const bool mtgb::InputUtil::GetMouseUp(const MouseCode _mouseCode, WindowContext _context)
{
	return false;
}
const bool mtgb::InputUtil::GetGamePad(const MouseCode _mouseCode, WindowContext _context)
{
	return false;
}
const bool mtgb::InputUtil::GetGamePadDown(const MouseCode _mouseCode, WindowContext _context)
{
	return false;
}
const bool mtgb::InputUtil::GetGamePadUp(const MouseCode _mouseCode, WindowContext _context)
{
	return false;
}
const mtgb::InputData& mtgb::InputUtil::GetInput(WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return *(Game::System<WindowContextResourceManager>().Get<InputResource>(WindowContext::First).GetInput());
	}
	return *(Game::System<WindowContextResourceManager>().Get<InputResource>(_context).GetInput());
}



const float mtgb::InputUtil::GetAxis(Axis axis,WindowContext _context = mtgb::WindowContext::Both)
{
	const InputData& input = GetInput(_context);
	float value = 0.0f;
	switch (axis)
	{
	case Axis::X: value = input.joyStateCurrent_.lX / input.config_.xRange; break;
	case Axis::Y: value = input.joyStateCurrent_.lY / input.config_.yRange; break;
	case Axis::Z: value = input.joyStateCurrent_.lZ / input.config_.zRange; break;
	default: return 0.0f;
	}
	return input.config_.ApplyDeadZone(value);
}

const mtgb::Vector2Int mtgb::InputUtil::GetMousePosition(WindowContext _context)
{
	return InputUtil::GetInput(_context).mousePosition_;
}

const mtgb::Vector3 mtgb::InputUtil::GetMouseMove(WindowContext _context)
{
	return mtgb::Vector3
	{
		static_cast<float>(InputUtil::GetInput(_context).mouseStateCurrent_.lX),
		static_cast<float>(InputUtil::GetInput(_context).mouseStateCurrent_.lY),
		static_cast<float>(InputUtil::GetInput(_context).mouseStateCurrent_.lZ),
	};
}

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



