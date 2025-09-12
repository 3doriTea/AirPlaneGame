#pragma once

#include <array>
#include "IncludingInput.h"
#include <bitset>

#include "Vector3.h"
#include "Vector2Int.h"
#include "Vector2F.h"
#include "InputKeyCode.h"
#include "InputMouseCode.h"
#include "InputPadCode.h"

#include "cmtgb.h"
#include "WindowContext.h"
#include "InputConfig.h"
#include "Input.h"
#include "Axis.h"

typedef struct HWND__* HWND;
namespace mtgb
{
	
	class Input;
	class InputUtil final
	{
	public:
		static const bool GetKey(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetKeyDown(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetKeyUp(const KeyCode _keyCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetMouse(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetMouseDown(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetMouseUp(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePad(const PadCode _padButtonCode,WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadDown(const PadCode _padButtonCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadUp(const PadCode _padButtonCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePad(const FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadDown(const FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadUp(const FlightStickCode _flightStickCode, WindowContext _context = mtgb::WindowContext::Both);
		//static inline const size_t* GetActiveGamePadID() { return activeGamepadID.data(); }

		static const float GetTrigger(FlightStickAxisCode _flightStickCode, WindowContext _context = mtgb::WindowContext::Both);
		static const float GetTrigger(PadAxisCode _padCode, WindowContext _context = mtgb::WindowContext::Both);
		static const float GetAxis(Axis axis,WindowContext _context = mtgb::WindowContext::Both);
		static const Vector2F GetAxis(WindowContext _context = mtgb::WindowContext::Both);

		static const Vector2Int GetMousePosition(WindowContext _context = mtgb::WindowContext::Both);
		static const Vector3 GetMouseMove(WindowContext _context = mtgb::WindowContext::Both);

	private:  // Utilities
		static const size_t KEY_COUNT{ 256 };             // キーの数
		/// <summary>
		/// currとprevのxorを取得
		/// </summary>
		/// <param name="_keyCode">キーコード</param>
		/// <returns>0: 差無し, 1: 差有り</returns>
		static inline const int KeyXOR(const KeyCode _keyCode,const std::bitset<KEY_COUNT>& _keyStateCurrent, const std::bitset<KEY_COUNT>& _keyStatePrevious)
		{
			return _keyStateCurrent[Index(_keyCode)] ^ _keyStatePrevious[Index(_keyCode)];
		}
		
		static inline const int MouseXOR(const MouseCode _mouseCode, const _DIMOUSESTATE& _mouseStateCurrent, const _DIMOUSESTATE& _mouseStatePrevious)
		{
			return _mouseStateCurrent.rgbButtons[Index(_mouseCode)] ^ _mouseStatePrevious.rgbButtons[Index(_mouseCode)];
		}

		/// <summary>
		/// キーコード構造体列挙型をインデックスに変換
		/// </summary>
		/// <param name="_keyCode">キーコード</param>
		/// <returns>キー配列のインデックス</returns>
		static inline const size_t Index(const KeyCode _keyCode)
		{
			return static_cast<size_t>(_keyCode);
		}

		static inline const size_t Index(const MouseCode _moudeCode)
		{
			return static_cast<size_t>(_moudeCode);
		}

		static inline const size_t Index(const PadCode _padCode)
		{
			return static_cast<size_t>(_padCode);
		}
		static inline const size_t Index(const FlightStickCode _flightStickCode)
		{
			return static_cast<size_t>(_flightStickCode);
		}

		/// <summary>
		/// 入力状態を取得
		/// どのウィンドウでも構わない場合はWindowContext::Firstのウィンドウが取得される
		/// </summary>
		/// <param name="_context">ウィンドウを指定</param>
		/// <returns></returns>
		static const InputData& GetInput(WindowContext _context);
	
		static inline const int PadXOR(const PadCode _padCode, const DIJOYSTATE& _padStateCurrent, const DIJOYSTATE& _padStatePrevious)
		{
			return _padStateCurrent.rgbButtons[Index(_padCode)] ^ _padStatePrevious.rgbButtons[Index(_padCode)];
		}
		//static inline const int padXOR(const PadCode _padCode, const _XINPUT_STATE& _padStateCurrent, const _XINPUT_STATE& _padStatePrevious)
		//{
		//	WORD buttonPrev = _padStatePrevious.Gamepad.wButtons; // 0000
		//	WORD buttonCurr = _padStateCurrent.Gamepad.wButtons;  // 1000
		//	WORD padCode    = static_cast<WORD>(_padCode);        // 0x08
		//	return (buttonCurr & padCode) ^ (buttonPrev & padCode); // 1 ^ 0 = 1
		//}

		static inline bool GetGamePadImpl(size_t _index, WindowContext _context);
		static inline bool GetGamePadUpImpl(size_t _index, WindowContext _context);
		static inline bool GetGamePadDownImpl(size_t _index, WindowContext _context);
	};

	class InputData final
	{
		friend Input;
		friend InputUtil;
		friend class InputResource;
	private:  // Data
		static const size_t KEY_COUNT{ 256 };      // キーの数
		std::bitset<KEY_COUNT> keyStateCurrent_;   // キーの状態現在
		std::bitset<KEY_COUNT> keyStatePrevious_;  // キーの状態前回
		
		_DIMOUSESTATE mouseStateCurrent_;   // マウスの状態現在
		_DIMOUSESTATE mouseStatePrevious_;  // マウスの状態前回
		DIJOYSTATE joyStateCurrent_;		// ジョイスティックの状態現在
		DIJOYSTATE joyStatePrevious_;		// ジョイスティックの状態現在

		InputConfig config_;//入力の取り方の設定

		Vector2Int mousePosition_;          // マウスカーソルの座標

		static const size_t GAME_PAD_COUNT{ XUSER_MAX_COUNT };            // ゲームパッドの最大接続可能数
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // ゲームパッドの状態現在
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // ゲームパッドの状態前回
		std::map<PadIDState ,int> activeGamePadID;                     // 有効なコントローラのID(0~3が入る)
		int gamePadID;                                                    // コンテキストに割り当てられたコントローラのID
	
	public:
		//InputData();
		//~InputData();
	};

	
}
