#include "InputData.h"
#include <xinput.h>
#include "Input.h"

const bool mtgb::InputData::GetKey(const KeyCode _keyCode)
{
	return keyStateCurrent_[Index(_keyCode)];
}

const bool mtgb::InputData::GetKeyDown(const KeyCode _keyCode)
{
	return static_cast<bool>(KeyXOR(_keyCode) & keyStateCurrent_[Index(_keyCode)]);
}

const bool mtgb::InputData::GetKeyUp(const KeyCode _keyCode)
{
	return static_cast<bool>(KeyXOR(_keyCode) & keyStateCurrent_[Index(_keyCode)]);
}

const bool mtgb::InputData::GetMouse(const MouseCode _mouseCode)
{
	
	return false;
}

const bool mtgb::InputData::GetMouseDown(const MouseCode _mouseCode)
{
	return false;
}

const bool mtgb::InputData::GetMouseUp(const MouseCode _mouseCode)
{
	return false;
}

const bool mtgb::InputData::GetGamePad(const MouseCode _mouseCode)
{
	return false;
}

const bool mtgb::InputData::GetGamePadDown(const MouseCode _mouseCode)
{
	return false;
}

const bool mtgb::InputData::GetGamePadUp(const MouseCode _mouseCode)
{
	return false;
}

const mtgb::Vector2Int mtgb::InputData::GetMousePosition()
{
	return mousePosition_;
}

const mtgb::Vector3 mtgb::InputData::GetMouseMove()
{
	return mtgb::Vector3
	{
		static_cast<float>(mouseStateCurrent_.lX),
		static_cast<float>(mouseStateCurrent_.lY),
		static_cast<float>(mouseStateCurrent_.lZ),
	};
}

std::bitset<mtgb::InputData::KEY_COUNT> mtgb::InputData::keyStateCurrent_{};
std::bitset<mtgb::InputData::KEY_COUNT> mtgb::InputData::keyStatePrevious_{};

_DIMOUSESTATE mtgb::InputData::mouseStateCurrent_{};
_DIMOUSESTATE mtgb::InputData::mouseStatePrevious_{};

mtgb::Vector2Int mtgb::InputData::mousePosition_{};

std::array<_XINPUT_STATE, mtgb::InputData::GAME_PAD_COUNT> mtgb::InputData::gamePadStateCurrent_{};
std::array<_XINPUT_STATE, mtgb::InputData::GAME_PAD_COUNT> mtgb::InputData::gamePadStatePrevious_{};
