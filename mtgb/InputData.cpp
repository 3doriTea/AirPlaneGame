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

const bool mtgb::InputUtil::GetGamePad(const PadCode _padButtonCode, const size_t _padID, WindowContext _context)
{

	if (_context == WindowContext::Both)
	{
		const InputData& inputFirstWnd{ GetInput(WindowContext::First) };
		const InputData& inputSecondWnd{ GetInput(WindowContext::Second) };

		// ? padIdをどうやって指定すればいいかわかんない。
		// static変数で持てばいいのか？

		// return inputFirstWnd.gamePadStateCurrent_[;
	}

	return GetInput(_context).gamePadStateCurrent_[_padID].Gamepad.wButtons & static_cast<WORD>(_padButtonCode);
}

const bool mtgb::InputUtil::GetGamePadDown(const PadCode _padButtonCode, const size_t _padID, WindowContext _context)
{
	if (_context == WindowContext::Both)
	{
		const InputData& inputFirstWnd{ GetInput(WindowContext::First) };
		const InputData& inputSecondWnd{ GetInput(WindowContext::Second) };

		// ? padIdをどうやって指定すればいいかわかんない。
		// static変数で持てばいいのか？

		// return inputFirstWnd.gamePadStateCurrent_[;
	}

	const InputData& input = GetInput(_context);
	int padXor = padXOR(_padButtonCode, input.gamePadStateCurrent_[_padID], input.gamePadStatePrevious_[_padID]);
	int buttonCurr = static_cast<int>(input.gamePadStateCurrent_[_padID].Gamepad.wButtons & static_cast<WORD>(_padButtonCode));
	
	return static_cast<bool>(padXor & buttonCurr);
}

const bool mtgb::InputUtil::GetGamePadUp(const PadCode _padButtonCode, const size_t _padID, WindowContext _context)
{

	if (_context == WindowContext::Both)
	{
		const InputData& inputFirstWnd{ GetInput(WindowContext::First) };
		const InputData& inputSecondWnd{ GetInput(WindowContext::Second) };

		// ? padIdとWindowContextをどう結びつければいいかわかんない。
		// inputdataに変数で持てばいいのか？
		// GetContextPadIDを作って、引数でIDは渡さないようにすれば良い。
		// DispatchPadID関数とか？


		// return inputFirstWnd.gamePadStateCurrent_[;
	}
	const InputData& input = GetInput(_context);
	int padXor = padXOR(_padButtonCode, input.gamePadStateCurrent_[_padID], input.gamePadStatePrevious_[_padID]);
	int buttonPrev = static_cast<int>(input.gamePadStatePrevious_[_padID].Gamepad.wButtons & static_cast<WORD>(_padButtonCode));
	return static_cast<bool>(padXor & buttonPrev);
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
