#pragma once
#define DIRECTINPUT_VERSION 0x0800

#include <array>
#include <bitset>

#include "Vector3.h"
#include "Vector2Int.h"

#include "InputKeyCode.h"
#include "InputMouseCode.h"
#include "cmtgb.h"

#include "Input.h"

struct _DIMOUSESTATE;
struct _XINPUT_STATE;

namespace mtgb
{
	class Input;

	class InputData final
	{
		friend Input;

	public:  // Data getter
		static const bool GetKey(const KeyCode _keyCode);
		static const bool GetKeyDown(const KeyCode _keyCode);
		static const bool GetKeyUp(const KeyCode _keyCode);
		
		static const bool GetMouse(const MouseCode _mouseCode);
		static const bool GetMouseDown(const MouseCode _mouseCode);
		static const bool GetMouseUp(const MouseCode _mouseCode);
		
		static const bool GetGamePad(const MouseCode _mouseCode);
		static const bool GetGamePadDown(const MouseCode _mouseCode);
		static const bool GetGamePadUp(const MouseCode _mouseCode);


		static const Vector2Int GetMousePosition();
		static const Vector3 GetMouseMove();

	private:  // Utilities
		/// <summary>
		/// currとprevのxorを取得
		/// </summary>
		/// <param name="_keyCode">キーコード</param>
		/// <returns>0: 差無し, 1: 差有り</returns>
		static inline const int KeyXOR(const KeyCode _keyCode)
		{
			return keyStateCurrent_[Index(_keyCode)] ^ keyStatePrevious_[Index(_keyCode)];
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

	private:  // Data
		static const size_t KEY_COUNT{ 256 };             // キーの数
		static std::bitset<KEY_COUNT> keyStateCurrent_;   // キーの状態現在
		static std::bitset<KEY_COUNT> keyStatePrevious_;  // キーの状態前回

		static _DIMOUSESTATE mouseStateCurrent_;   // マウスの状態現在
		static _DIMOUSESTATE mouseStatePrevious_;  // マウスの状態前回
		static Vector2Int mousePosition_;          // マウスカーソルの座標

		static const size_t GAME_PAD_COUNT{ 4 };  // ゲームパッドの最大接続可能数
		static std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // ゲームパッドの状態現在
		static std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // ゲームパッドの状態前回

	private:
		InputData() = delete;
		~InputData() = delete;
	};
}
