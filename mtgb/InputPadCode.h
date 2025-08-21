#pragma once

#include "IncludingInput.h"

/// <summary>
/// ゲームパッドのボタンコード
/// </summary>
enum struct PadCode : WORD
{
	Up      = XINPUT_GAMEPAD_DPAD_UP,
	Down    = XINPUT_GAMEPAD_DPAD_DOWN,
	Left    = XINPUT_GAMEPAD_DPAD_LEFT,
	Right   = XINPUT_GAMEPAD_DPAD_RIGHT,
	Start   = XINPUT_GAMEPAD_START,
	Back    = XINPUT_GAMEPAD_BACK, // いわゆるselectボタンのこと。
	LStick  = XINPUT_GAMEPAD_LEFT_THUMB,
	RStick  = XINPUT_GAMEPAD_RIGHT_THUMB,
	Lbutton = XINPUT_GAMEPAD_LEFT_SHOULDER,
	Rbutton = XINPUT_GAMEPAD_RIGHT_SHOULDER,
	A       = XINPUT_GAMEPAD_A,
	B       = XINPUT_GAMEPAD_B,
	X       = XINPUT_GAMEPAD_X,
	Y       = XINPUT_GAMEPAD_Y,
};

enum struct PadIDState
{
	INVALID,    // 無効
	ASSIGNED,   // 割り当て済み
	UNASSIGNED, // 空き
};