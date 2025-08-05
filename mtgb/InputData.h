#pragma once

#include <array>
#include "IncludingInput.h"
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
	enum class Axis
	{
		X,
		Y,
		Z
	};
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
		static const bool GetGamePad(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadDown(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);
		static const bool GetGamePadUp(const MouseCode _mouseCode, WindowContext _context = mtgb::WindowContext::Both);

		
		static const float GetAxis(Axis axis,WindowContext _context = mtgb::WindowContext::Both);

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

		/// <summary>
		/// 入力状態を取得
		/// どのウィンドウでも構わない場合はWindowContext::Firstのウィンドウが取得される
		/// </summary>
		/// <param name="_context">ウィンドウを指定</param>
		/// <returns></returns>
		static const InputData& GetInput(WindowContext _context);
	};

	struct InputConfig
	{

		LONG xRange;//x軸の値の範囲
		LONG yRange;//y軸の値の範囲
		LONG zRange;//z軸の値の範囲

		float deadZone;//入力を無視する閾値

		/// <summary>
		/// x,y,z軸すべてに範囲の設定
		/// 下限が負、上限が正
		/// </summary>
		/// <param name="_range">範囲となる絶対値</param>
		void SetRange(LONG _range);

		/// <summary>
		/// 指定した軸に範囲の設定
		/// 下限が負、上限が正
		/// </summary>
		/// <param name="_range">範囲となる絶対値</param>
		/// <param name="_axis">設定する軸</param>
		void SetRange(LONG _range, Axis _axis);

		/// <summary>
		/// 指定した値(割合)でデッドゾーンを設定
		/// </summary>
		/// <param name="_deadZone">0～1の範囲で指定してください</param>
		void SetDeadZone(float _deadZone);

		
		/// <summary>
		/// デッドゾーンを適用する
		/// </summary>
		/// <param name="value">適用する値</param>
		/// <returns>適用された値</returns>
		float ApplyDeadZone(const float value) const;
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
		DIJOYSTATE joyStateCurrent_;		// ジョイスティックの状態現在
		DIJOYSTATE joyStatePrevious_;		// ジョイスティックの状態現在

		InputConfig config_;//入力の取り方の設定

		Vector2Int mousePosition_;          // マウスカーソルの座標

		static const size_t GAME_PAD_COUNT{ 4 };  // ゲームパッドの最大接続可能数
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStateCurrent_;   // ゲームパッドの状態現在
		std::array<_XINPUT_STATE, GAME_PAD_COUNT> gamePadStatePrevious_;  // ゲームパッドの状態前回

	public:
		//InputData();
		//~InputData();
	};

	
}
