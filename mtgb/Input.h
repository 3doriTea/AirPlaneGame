#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include <functional>
#include <vector>
#include <tuple>
#include <set>

#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "Xinput.lib")

typedef struct IDirectInput8A* LPDIRECTINPUT8;
typedef struct IDirectInputDevice8A* LPDIRECTINPUTDEVICE8;
typedef struct HWND__* HWND;

namespace mtgb
{
	class InputResource;
	class InputData;
	class Input : public ISystem
	{
	public:
		Input();
		~Input();

		void Initialize() override;
		void Update() override;

		void Release();

		/// <summary>
		/// マウスの座標データを更新する
		/// </summary>
		/// <param name="_x">座標 x</param>
		/// <param name="_y">座標 y</param>
		void UpdateMousePositionData(const int32_t _x, const int32_t _y);

		void CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice);
		void CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice);
		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のキーボードデバイス</param>
		void ChangeKeyDevice(LPDIRECTINPUTDEVICE8 _pKeyDevice);
		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のマウスデバイス</param>
		void ChangeMouseDevice(LPDIRECTINPUTDEVICE8 _pMouseDevice);
		/// <summary>
		/// 入力状態を格納する対象を切り返す
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象の入力状態を格納する物</param>
		void ChangeInputData(InputData* _pInputData);
		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のジョイスティックデバイス</param>
		void ChangeJoystickDevice(LPDIRECTINPUTDEVICE8 _pJoystickDevice);

		/// <summary>
		/// 接続されているジョイスティックの列挙、予約デバイスへの割り当てを行う
		/// </summary>
		void EnumJoystick();

		/// <summary>
		/// ジョイスティックが接続された場合に割り当てられるよう予約する
		/// 先着順で割り当てられます
		/// </summary>
		/// <param name="_pJoystickDevice">割り当て希望のデバイス</param>
		void RequestJoystickDevice(HWND _hWnd, InputConfig _inputConfig, LPDIRECTINPUTDEVICE8 _pJoystickDevice);

		/// <summary>
		/// 接続されているジョイスティックを割り当て予約してるデバイスに割り当てる
		/// </summary>
		/// <param name="_pJoystickDevice"></param>
		void AssignJoystick(LPDIRECTINPUTDEVICE8 _pJoystickDevice);

		/// <summary>
		/// 割り当てられたジョイスティックのGUIDを登録する
		/// </summary>
		/// <param name="_guid">登録するジョイスティックのGUID</param>
		/// <returns>登録済みの場合はfalseを返す</returns>
		bool RegisterJoystickGuid(GUID _guid);
		/// <summary>
		/// 割り当て予約がされていないか否か
		/// </summary>
		/// <returns>/returns>
		bool IsNotSubscribed();

	private:
		void SetProperty(LPDIRECTINPUTDEVICE8 _pJoystickDevice, InputConfig _inputConfig);
		InputData* pInputData_;				 // 入力の状態
		LPDIRECTINPUT8 pDirectInput_;        // Direct Input 本体k
		LPDIRECTINPUTDEVICE8 pKeyDevice_;    // キーデバイス
		LPDIRECTINPUTDEVICE8 pMouseDevice_;  // マウスデバイス
		LPDIRECTINPUTDEVICE8 pJoystickDevice_;  // ジョイスティックデバイス
		 
		std::vector<std::tuple<HWND,InputConfig, LPDIRECTINPUTDEVICE8>> requestedJoystickDevices_;//割り当て予約されたジョイスティックデバイス
		std::set<GUID> assignedJoystickGuids_;//既に割り当て済みのジョイスティック
	};
}
