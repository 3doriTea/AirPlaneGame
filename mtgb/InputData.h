#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <array>
#include <Xinput.h>
#include <dinput.h>
#include <bitset>

#include "Vector3.h"
#include "Vector2Int.h"

#include "InputKeyCode.h"
#include "InputMouseCode.h"
#include "cmtgb.h"
#include "WindowContext.h"

#include "Input.h"


typedef struct HWND__* HWND;
namespace mtgb
{
	class Input;
	class InputUtil final
	{
	public:
		static const bool GetKey(const KeyCode _keyCode,WindowContext _context);
		static const bool GetKeyDown(const KeyCode _keyCode,WindowContext _context);
		static const bool GetKeyUp(const KeyCode _keyCode,WindowContext _context);
		static const bool GetMouse(const MouseCode _mouseCode,WindowContext _context);
		static const bool GetMouseDown(const MouseCode _mouseCode,WindowContext _context);
		static const bool GetMouseUp(const MouseCode _mouseCode,WindowContext _context);
		static const bool GetGamePad(const MouseCode _mouseCode,WindowContext _context);
		static const bool GetGamePadDown(const MouseCode _mouseCode,WindowContext _context);
		static const bool GetGamePadUp(const MouseCode _mouseCode,WindowContext _context);


		static const Vector2Int GetMousePosition(WindowContext _context);
		static const Vector3 GetMouseMove(WindowContext _context);

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
		
		/// <summary>
		/// キーコード構造体列挙型をインデックスに変換
		/// </summary>
		/// <param name="_keyCode">キーコード</param>
		/// <returns>キー配列のインデックス</returns>
		static inline const size_t Index(const KeyCode _keyCode)
		{
			return static_cast<size_t>(_keyCode);
		}

		static const InputData& GetInput(WindowContext _context);
	};
	class InputData final
	{
		friend Input;
		friend InputUtil;
	private:  // Data
		static const size_t KEY_COUNT{ 256 };             // キーの数
		std::bitset<KEY_COUNT> keyStateCurrent_;   // キーの状態現在
		std::bitset<KEY_COUNT> keyStatePrevious_;  // キーの状態前回

		_DIMOUSESTATE mouseStateCurrent_;   // マウスの状態現在
		_DIMOUSESTATE mouseStatePrevious_;  // マウスの状態前回
		Vector2Int mousePosition_;          // マウスカーソルの座標

		static const size_t GAME_PAD_COUNT{ 4 };  // ゲームパッドの最大接続可能数
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // ゲームパッドの状態現在
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // ゲームパッドの状態前回

	public:
		//InputData();
		//~InputData();
	};
}
