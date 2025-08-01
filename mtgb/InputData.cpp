#include "InputData.h"

#include "Input.h"
#include "WindowContextResourceManager.h"
#include "InputResource.h"
#include "Game.h"
#include "ISystem.h"


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

		return static_cast<bool>(KeyXOR(_keyCode, inputFirstWnd.keyStateCurrent_, inputFirstWnd.keyStatePrevious_) & inputFirstWnd.keyStateCurrent_[Index(_keyCode)])
			|| static_cast<bool>(KeyXOR(_keyCode, inputSecondWnd.keyStateCurrent_, inputSecondWnd.keyStatePrevious_) & inputSecondWnd.keyStateCurrent_[Index(_keyCode)]);
	}

	const InputData& input = GetInput(_context);
	return static_cast<bool>(KeyXOR(_keyCode, input.keyStateCurrent_, input.keyStatePrevious_) & input.keyStateCurrent_[Index(_keyCode)]);
}
const bool mtgb::InputUtil::GetKeyUp(const KeyCode _keyCode, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		const InputData& inputFirstWnd = GetInput(WindowContext::First);
		const InputData& inputSecondWnd = GetInput(WindowContext::Second);

		return static_cast<bool>(KeyXOR(_keyCode, inputFirstWnd.keyStateCurrent_, inputFirstWnd.keyStatePrevious_) & inputFirstWnd.keyStatePrevious_[Index(_keyCode)])
			|| static_cast<bool>(KeyXOR(_keyCode, inputSecondWnd.keyStateCurrent_, inputSecondWnd.keyStatePrevious_) & inputSecondWnd.keyStatePrevious_[Index(_keyCode)]);
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




