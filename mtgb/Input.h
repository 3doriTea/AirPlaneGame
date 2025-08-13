#pragma once
#include "ISystem.h"
#include "cmtgb.h"
#include "IncludingInput.h"
#include <wrl/client.h> // 追加
#include <functional>
#include <vector>
#include <tuple>
#include <set>
#include <guiddef.h>
#include <map>
#include "Timer.h"
#include "InputConfig.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")

typedef struct HWND__* HWND;
using Microsoft::WRL::ComPtr; // 追加



namespace mtgb
{
	class InputResource;
	class InputData;

	struct JoystickContext
	{
		TimerHandle timerHandle;
		HRESULT lastResult;
		ComPtr<IDirectInputDevice8> device;
		JoystickContext() = default;
		JoystickContext(IDirectInputDevice8* _device);
	};

	/// <summary>
	/// ジョイスティックの割り当て要求
	/// </summary>
	struct JoystickReservation
	{
		HWND hWnd;
		InputConfig config;
		std::function<void(ComPtr<IDirectInputDevice8>, GUID)> onAssign;
	};

	class Input : public ISystem
	{
	public:
		Input();
		~Input();

		void Initialize() override;
		void Update() override;
		void UpdateKeyDevice();
		void UodateMouseDevice();
		void UpdateJoystickDevice();

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
		void ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice);

		void SetJoystickGuid(GUID _guid);

		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のマウスデバイス</param>
		void ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice);
		/// <summary>
		/// 入力状態を格納する対象を切り返す
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象の入力状態を格納する物</param>
		void ChangeInputData(InputData* _pInputData);
		/// <summary>
		/// 入力状態を取得する対象のデバイスを切り替える
		/// </summary>
		/// <param name="_pJoystickDevice">切り替え対象のジョイスティックデバイス</param>
		void ChangeJoystickDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice);

		/// <summary>
		/// 接続されているジョイスティックの列挙、予約デバイスへの割り当てを行う
		/// </summary>
		void EnumJoystick();

		/// <summary>
		/// ジョイスティックが接続された場合に割り当てられるよう予約する
		/// 先着順で割り当てられます
		/// </summary>
		/// <param name="_pJoystickDevice">割り当て希望のデバイス</param>
		void RequestJoystickDevice(JoystickReservation* _reservation);

		/// <summary>
		/// 接続されているジョイスティックを割り当て予約してるデバイスに割り当てる
		/// </summary>
		/// <param name="_pJoystickDevice"></param>
		void AssignJoystick(IDirectInputDevice8* _pJoystickDevice);

		/// <summary>
		/// 登録されたジョイスティックを解除する
		/// </summary>
		/// <param name="_guid">登録解除するGUID</param>
		/// <returns></returns>
		void UnregisterJoystickGuid(GUID _guid);

		/// <summary>
		/// 割り当てられたジョイスティックのGUIDを登録する
		/// </summary>
		/// <param name="_guid">登録するジョイスティックのGUID</param>
		/// <returns>登録済みの場合はfalseを返す</returns>
		bool RegisterJoystickGuid(GUID _guid);

		/// <summary>
		/// 取得感覚を設定する
		/// </summary>
		void SetAcquireInterval(GUID _guid, ComPtr<IDirectInputDevice8> _device);
		/// <summary>
		/// 割り当て予約がされていないか否か
		/// </summary>
		/// <returns>/returns>
		bool IsNotSubscribed();

		std::string ConvertHResultToMessage(HRESULT hr) const;

		HRESULT UpdateJoystickState(GUID guid);

		const std::string GetJoystickStatusMessage(GUID guid) const;
		bool IsJoystickConnected(GUID guid) const;
		bool IsJoystickAssigned(GUID guid) const;

	private:
		void AcquireJoystick(ComPtr<IDirectInputDevice8> _pJoystickDevice);
		GUID GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice);
		void SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig);
		InputData* pInputData_;				 // 入力の状態
		ComPtr<IDirectInput8> pDirectInput_;        // Direct Input 本体k
		ComPtr<IDirectInputDevice8> pKeyDevice_;    // キーデバイス
		ComPtr<IDirectInputDevice8> pMouseDevice_;  // マウスデバイス
		ComPtr<IDirectInputDevice8> pJoystickDevice_;  // ジョイスティックデバイス
		 
		std::vector<JoystickReservation*> requestedJoystickDevices_;//割り当て予約されたジョイスティックデバイス
		std::set<GUID> assignedJoystickGuids_;//既に割り当て済みのジョイスティック
		
		std::map<GUID, JoystickContext> joystickContext_;
		GUID currJoystickGuid_;
	};
	
}
