#include "InputData.h"
#include <xinput.h>
#include "Input.h"
#include "WindowContextResourceManager.h"
#include "InputResource.h"
#include "Game.h"
#include "ISystem.h"


const bool mtgb::InputUtil::GetKey(const KeyCode _keyCode, HWND _hWnd)
{
	return InputData(_hWnd).keyStateCurrent_[Index(_keyCode)];
}
const bool mtgb::InputUtil::GetKeyDown(const KeyCode _keyCode, HWND _hWnd)
{
	return static_cast<bool>(KeyXOR(_keyCode, InputData(_hWnd).keyStateCurrent_, InputData(_hWnd).keyStatePrevious_) & InputData(_hWnd).keyStateCurrent_[Index(_keyCode)]);
}
const bool mtgb::InputUtil::GetKeyUp(const KeyCode _keyCode, HWND _hWnd)
{
	return static_cast<bool>(KeyXOR(_keyCode, InputData(_hWnd).keyStateCurrent_, InputData(_hWnd).keyStatePrevious_) & InputData(_hWnd).keyStatePrevious_[Index(_keyCode)]);

}
const bool mtgb::InputUtil::GetMouse(const MouseCode _mouseCode, HWND _hWnd)
{
	return false;
}
const bool mtgb::InputUtil::GetMouseDown(const MouseCode _mouseCode, HWND _hWnd)
{
	return false;
}
const bool mtgb::InputUtil::GetMouseUp(const MouseCode _mouseCode, HWND _hWnd)
{
	return false;
}
const bool mtgb::InputUtil::GetGamePad(const MouseCode _mouseCode, HWND _hWnd)
{
	return false;
}
const bool mtgb::InputUtil::GetGamePadDown(const MouseCode _mouseCode, HWND _hWnd)
{
	return false;
}
const bool mtgb::InputUtil::GetGamePadUp(const MouseCode _mouseCode, HWND _hWnd)
{
	return false;
}
const mtgb::InputData& mtgb::InputUtil::InputData(HWND _hWnd)
{
    return *(Game::System<WindowContextResourceManager>().Get<InputResource>(_hWnd).inputData_);
}



const mtgb::Vector2Int mtgb::InputUtil::GetMousePosition(HWND _hWnd)
{
	return InputUtil::InputData(_hWnd).mousePosition_;
}

const mtgb::Vector3 mtgb::InputUtil::GetMouseMove(HWND _hWnd)
{
	return mtgb::Vector3
	{
		static_cast<float>(InputData(_hWnd).mouseStateCurrent_.lX),
		static_cast<float>(InputData(_hWnd).mouseStateCurrent_.lY),
		static_cast<float>(InputData(_hWnd).mouseStateCurrent_.lZ),
	};
}




