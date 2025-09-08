#pragma once

#include "IncludingInput.h"

/// <summary>
/// ゲームパッドのボタンコード
/// </summary>
enum struct XInputPadCode : WORD
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

/// <summary>
/// ゲームパッドのDirectInputでのボタンコード
/// </summary>
enum struct PadCode : uint8_t
{
	A = 0,
	B = 1,
	X = 2,
	Y = 3,
	LB = 4,
	RB = 5,
	Back = 6,
	LStick = 6,
	RStick = 7,
	Start = 9,
};


/// <summary>
/// <para> フライトスティックのボタンコード </para>
/// <para> どのインデックスがどのボタンかデバイスによって異なる可能性があるので注意 </para>
/// </summary>
enum struct FlightStickCode : uint8_t
{
	Thumb = 0, // 親指
	Trigger = 1,
	Button3 = 2,
	Button4 = 3,
};

enum struct PadIDState
{
	INVALID,    // 無効
	ASSIGNED,   // 割り当て済み
	UNASSIGNED, // 空き
};