#include "InputData.h"

#include "Input.h"
#include "WindowContextResourceManager.h"
#include "InputResource.h"
#include "Game.h"
#include "ISystem.h"
#include "JoystickProxy.h"
#include <limits>


namespace
{

	constexpr LONG JOY_AXIS_MAX = 65535;
}

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
	int result{ KeyXOR(_keyCode, input.keyStateCurrent_, input.keyStatePrevious_) & static_cast<int>(input.keyStatePrevious_[Index(_keyCode)]) };
	return static_cast<bool>(result);
}

const bool mtgb::InputUtil::GetMouse(const MouseCode _mouseCode, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return GetMouse(_mouseCode, WindowContext::First) || GetMouse(_mouseCode, WindowContext::Second);
	}
	return GetInput(_context).mouseStateCurrent_.rgbButtons[Index(_mouseCode)] & 0x80;
}

const bool mtgb::InputUtil::GetMouseDown(const MouseCode _mouseCode, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return GetMouseDown(_mouseCode, WindowContext::First) || GetMouseDown(_mouseCode, WindowContext::Second);
	}
	const InputData& input = GetInput(_context);
	return static_cast<bool>(MouseXOR(_mouseCode, input.mouseStateCurrent_, input.mouseStatePrevious_) & static_cast<int>(input.mouseStateCurrent_.rgbButtons[Index(_mouseCode)]));
}

const bool mtgb::InputUtil::GetMouseUp(const MouseCode _mouseCode, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return GetMouseUp(_mouseCode, WindowContext::First) || GetMouseUp(_mouseCode, WindowContext::Second);
	}

	const InputData& input = GetInput(_context);
	return static_cast<bool>(MouseXOR(_mouseCode, input.mouseStateCurrent_, input.mouseStatePrevious_) & static_cast<int>(input.mouseStatePrevious_.rgbButtons[Index(_mouseCode)]));
}

const bool mtgb::InputUtil::GetGamePad(const PadCode _padButtonCode, WindowContext _context)
{
	return GetGamePadImpl(Index(_padButtonCode), _context);
}

const bool mtgb::InputUtil::GetGamePadDown(const PadCode _padButtonCode, WindowContext _context)
{
	return GetGamePadDownImpl(Index(_padButtonCode), _context);
}

const bool mtgb::InputUtil::GetGamePadUp(const PadCode _padButtonCode, WindowContext _context)
{
	return GetGamePadUpImpl(Index(_padButtonCode), _context);
}

const bool mtgb::InputUtil::GetGamePad(const FlightStickCode _flightStickCode, WindowContext _context)
{
	return GetGamePadImpl(Index(_flightStickCode), _context);
}

const bool mtgb::InputUtil::GetGamePadDown(const FlightStickCode _flightStickCode, WindowContext _context)
{
	return GetGamePadDownImpl(Index(_flightStickCode), _context);
}

const bool mtgb::InputUtil::GetGamePadUp(const FlightStickCode _flightStickCode, WindowContext _context)
{
	return  GetGamePadUpImpl(Index(_flightStickCode), _context);
}

const mtgb::InputData& mtgb::InputUtil::GetInput(WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return *(Game::System<WindowContextResourceManager>().Get<InputResource>(WindowContext::First).GetInput());
	}
	return *(Game::System<WindowContextResourceManager>().Get<InputResource>(_context).GetInput());
}

bool mtgb::InputUtil::GetGamePadImpl(size_t _index, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return GetGamePadImpl(_index, WindowContext::First) || GetGamePadImpl(_index, WindowContext::Second);
	}
	return GetInput(_context).joyStateCurrent_.rgbButtons[_index];
}

bool mtgb::InputUtil::GetGamePadUpImpl(size_t _index, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return GetGamePadUpImpl(_index, WindowContext::First) || GetGamePadUpImpl(_index, WindowContext::Second);
	}

	const InputData& input = GetInput(_context);
	int XOR = input.joyStateCurrent_.rgbButtons[_index] ^ input.joyStatePrevious_.rgbButtons[_index];
	int prev = input.joyStatePrevious_.rgbButtons[_index];
	return static_cast<bool>(XOR & prev);
}

bool mtgb::InputUtil::GetGamePadDownImpl(size_t _index, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		return GetGamePadDownImpl(_index, WindowContext::First) || GetGamePadDownImpl(_index, WindowContext::Second);
	}

	const InputData& input = GetInput(_context);
	int XOR = input.joyStateCurrent_.rgbButtons[_index] ^ input.joyStatePrevious_.rgbButtons[_index];
	int curr = input.joyStateCurrent_.rgbButtons[_index];
	return static_cast<bool>(XOR & curr);
}





const float mtgb::InputUtil::GetTrigger(FlightStickAxisCode _flightStickCode, WindowContext _context)
{
	const InputData& input = GetInput(_context);
	switch (_flightStickCode)
	{
	case FlightStickAxisCode::Slider: return static_cast<float>(input.joyStateCurrent_.rglSlider[0]) / JOY_AXIS_MAX;
	default: return 0.0f;
	}
}

const float mtgb::InputUtil::GetTrigger(PadAxisCode _padCode, WindowContext _context)
{
	const InputData& input = GetInput(_context);

	switch (_padCode)
	{
	case PadAxisCode::LeftTrigger: return static_cast<float>(input.joyStateCurrent_.lRy) / JOY_AXIS_MAX;
	case PadAxisCode::RightTrigger: return static_cast<float>(input.joyStateCurrent_.lRx) / JOY_AXIS_MAX;
	default: return 0.0f;
	}
	
}

const float mtgb::InputUtil::GetAxis(Axis axis,WindowContext _context)
{
	const InputData& input = GetInput(_context);
	float value = 0.0f;
	switch (axis)
	{
	case Axis::X: value = static_cast<float>(input.joyStateCurrent_.lX) / input.config_.xRange; break;
	case Axis::Y: value = static_cast<float>(input.joyStateCurrent_.lY) / input.config_.yRange; break;
	case Axis::Z: value = static_cast<float>(input.joyStateCurrent_.lZ) / input.config_.zRange; break;
	default: return 0.0f;
	}
	return input.config_.ApplyDeadZone(value);
}

const mtgb::Vector2F mtgb::InputUtil::GetAxis(WindowContext _context)
{
	const InputData& input = GetInput(_context);
	return Vector2F{ 
		GetAxis(Axis::X,_context),GetAxis(Axis::Y,_context)};
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
