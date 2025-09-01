#include "Input.h"
#include "IncludingWindows.h"
#include "IncludingInput.h"
#include "InputData.h"
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "MainWindow.h"
#include "DoubleWindow.h"
#include "InputResource.h"
#include <algorithm>
#include "Game.h"
#include "ISystem.h"
#include "Debug.h"
#include "../ImGui/imgui.h"
#include "Timer.h"

namespace
{
	static const size_t KEY_BUFFER_SIZE{ 256 };
	LONG min = -1000;
	LONG max = 1000;
	LONG xMin{ min },
		 xMax{ max },
		 yMin{ min },
		 yMax{ max };
	float acquireInterval = 10.0f;
}

using namespace mtgb;

void mtgb::Input::AcquireJoystick(ComPtr<IDirectInputDevice8> _pJoystickDevice)
{
	HRESULT hResult{};
	hResult = _pJoystickDevice->Acquire();
	joystickContext_[currJoystickGuid_].lastResult = hResult;
	switch (hResult)
	{
	case DI_OK:  //取得できた
	case S_FALSE://他のアプリも許可を取得している
		break;
	case DIERR_OTHERAPPHASPRIO://他のアプリが優先権を持っている
		return;
	case DIERR_INVALIDPARAM:
	case DIERR_NOTINITIALIZED:
		massert(SUCCEEDED(hResult)
			&& "ジョイスティック操作の許可取得の際にエラーが起こりました @Input::Update");
		return;
	default:
		break;
	}
}

GUID mtgb::Input::GetDeviceGuid(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult)
		&& "デバイスの情報の取得に失敗しました @Input::Update");
	return deviceInstance.guidInstance;
}

bool operator<(const GUID& lhs, const GUID& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(GUID)) < 0;
}


mtgb::Input::Input() :
	pInputData_  { nullptr },
	pDirectInput_{ nullptr },
	pKeyDevice_  { nullptr },
	pMouseDevice_{ nullptr }
{
}

mtgb::Input::~Input()
{
	Release();
	
}

void mtgb::Input::Initialize()
{
	HRESULT hResult{};

	// DirectInput8のデバイス作成
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(pDirectInput_.GetAddressOf()),
		nullptr);

	massert(SUCCEEDED(hResult)  // DirectInput8のデバイス作成に成功
		&& "DirectInput8のデバイス作成に失敗 @Input::Initialize");

}

void mtgb::Input::Update()
{
	static HRESULT hResult{};

#pragma region キーボード
	UpdateKeyDevice();
#pragma endregion

#pragma region マウス
	UpdateMouseDevice();
#pragma endregion

#pragma region ジョイスティック
	
	/*if (pJoystickDevice_ == nullptr)
	{
		return;
	}*/
	UpdateJoystickDevice();
	
#pragma endregion
	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		EnumJoystick();
	}

#pragma region ゲームパッド
	//UpdateGamePadDevice();
#pragma endregion
}

void mtgb::Input::UpdateKeyDevice()
{
	static HRESULT hResult{};
	// キーボード操作の許可ゲット
	hResult = pKeyDevice_->Acquire();

	//massert(SUCCEEDED(hResult)  // キーボード操作の許可取得に成功
	//	&& "キーボード操作の許可取得に失敗 @Input::Update");

	if (FAILED(hResult))
	{
		return;  // キーボード操作の許可取得に失敗したなら回帰
	}

	static BYTE keyBuffer[KEY_BUFFER_SIZE]{};  // キー状態取得用バッファ

	pInputData_->keyStatePrevious_ = pInputData_->keyStateCurrent_;
	pKeyDevice_->GetDeviceState(KEY_BUFFER_SIZE, keyBuffer);

	// TODO: forで回すのはコスパよくない
	for (int i = 0; i < KEY_BUFFER_SIZE; i++)
	{
		pInputData_->keyStateCurrent_[i] = keyBuffer[i];
	}
}

void mtgb::Input::UpdateMouseDevice()
{
	static HRESULT hResult{};

	// マウス操作の許可をゲット
	hResult = pMouseDevice_->Acquire();

	if (FAILED(hResult))
	{
		return;  // マウス操作の許可取得に失敗したなら回帰
	}

	massert(SUCCEEDED(hResult)  // マウス操作の許可取得に成功
		&& "マウス操作の許可取得に失敗 @Input::Update");

	memcpy(
		&pInputData_->mouseStatePrevious_,
		&pInputData_->mouseStateCurrent_,
		sizeof(DIMOUSESTATE));


	hResult = pMouseDevice_->GetDeviceState(
		sizeof(DIMOUSESTATE),
		&pInputData_->mouseStateCurrent_);

	massert(SUCCEEDED(hResult)  // マウス操作の取得に成功
		&& "マウス操作の取得に失敗 @Input::Update");

#pragma endregion
}

void mtgb::Input::UpdateJoystickDevice()
{
	static HRESULT hResult{};
	
	if (joystickContext_.empty()) return;
	if (currJoystickGuid_ == GUID_NULL) return;

	memcpy(
		&pInputData_->joyStatePrevious_,
		&pInputData_->joyStateCurrent_,
		sizeof(DIJOYSTATE));

	hResult = joystickContext_[currJoystickGuid_].device->GetDeviceState(sizeof(DIJOYSTATE), &pInputData_->joyStateCurrent_);
	joystickContext_[currJoystickGuid_].lastResult = hResult;
	switch (hResult)
	{
	case DI_OK:
		// LOGF("OK\n");
		break;
	case DIERR_INPUTLOST:// 入力ロスト、一時的なアクセス不可
		AcquireJoystick(joystickContext_[currJoystickGuid_].device);
		return;
	case DIERR_NOTACQUIRED:// 未取得
		AcquireJoystick(joystickContext_[currJoystickGuid_].device);
		return;
	default: // 何らかの失敗
	{
		// デバイスを割り当て済みリストから除外
		UnregisterJoystickGuid(GetDeviceGuid(joystickContext_[currJoystickGuid_].device));
		return;
	}
	/* massert(false
		&& "デバイスの状態の取得の際にエラーが起こりました @Input::Update");*/
	}
}

void mtgb::Input::UpdateGamePadDevice()
{
	// TODO: 関数化せよ！

	// アクティブなコントローラがなければ、リターン。
	{
		bool IS_GAMEPAD_DETECTED = std::any_of(pInputData_->activeGamePadID.begin(),
											   pInputData_->activeGamePadID.end(),
											   [](std::pair<const PadIDState, int> _id) { return _id.second != -1; });
		if (not(IS_GAMEPAD_DETECTED))
		{
			CheckValidPadID();
		}
	}

	// コントローラの割り当て
	// 無効なIDであれば書き換え
	// 割り当てたIDのキーをASSIGNEDにする
	// 

	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		// PreviousにCurrentの状態をコピー
		memcpy(
			&pInputData_->gamePadStatePrevious_[i],
			&pInputData_->gamePadStateCurrent_[i],
			sizeof(_XINPUT_STATE));

		// 現在のコントローラーの状態を取得
		XInputGetState(i, &pInputData_->gamePadStateCurrent_[i]); // ここでエラー処理!
	}
}

void mtgb::Input::Release()
{
	pMouseDevice_.Reset();
	pKeyDevice_.Reset();
	pDirectInput_.Reset();
}

void mtgb::Input::UpdateMousePositionData(
	const int32_t _x,
	const int32_t _y)
{
	if (pInputData_)
	{
		pInputData_->mousePosition_.x = _x;
		pInputData_->mousePosition_.y = _y;
	}
}

void mtgb::Input::CreateKeyDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppKeyDevice)
{
	HRESULT hResult{};

	hResult = pDirectInput_->CreateDevice(GUID_SysKeyboard, _ppKeyDevice, nullptr);
	massert(SUCCEEDED(hResult)  // キーボードデバイスの作成に成功
		&& "キーボードデバイスの作成に失敗 @Input::CreateKeyDevice");

	// キーボード用にフォーマット
	hResult = (*_ppKeyDevice)->SetDataFormat(&c_dfDIKeyboard);

	massert(SUCCEEDED(hResult)  // キーボードフォーマットに成功
		&& "キーボードフォーマットに失敗 @Input::CreateDevice");

	// キーボードのアプリ間共有レベルを設定
	//  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	//hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//非アクティブなアプリも入力を受け付ける
	hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(SUCCEEDED(hResult)  // キーボードアプリ間共有レベル設定に成功
		&& "キーボードアプリ間共有レベル設定に失敗 @Input::CreateDevice");
}

void mtgb::Input::CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice)
{
	HRESULT hResult{};

	hResult = pDirectInput_->CreateDevice(GUID_SysMouse, _ppMouseDevice, nullptr);
	massert(SUCCEEDED(hResult)  // キーボードデバイスの作成に成功
		&& "マウスデバイスの作成に失敗 @Input::CreateMouseDevice");

	// マウス用にフォーマット
	hResult = (*_ppMouseDevice)->SetDataFormat(&c_dfDIMouse);

	massert(SUCCEEDED(hResult)  // マウスフォーマットに成功
		&& "マウスフォーマットに失敗 @Input::CreateMouseDevice");

	// マウスのアプリ間共有レベルの設定
	//hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//非アクティブなアプリも入力を受け付ける
	hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(SUCCEEDED(hResult)  // マウスアプリ間共有レベル設定に成功
		&& "マウスアプリ間共有レベル設定に失敗 @Input::CreateMouseDevice");
}

void mtgb::Input::ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice)
{
	pKeyDevice_ = _pKeyDevice;
}

void mtgb::Input::SetJoystickGuid(GUID _guid)
{
	/*massert(assignedJoystickGuids_.contains(_guid)
		&& "無効なGUIDが渡されました @Input::SetJoystickGuid");*/
	currJoystickGuid_ = _guid;
}

void mtgb::Input::ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice)
{
	pMouseDevice_ = _pMouseDevice;
}

void mtgb::Input::ChangeInputData(InputData* _pInputData)
{
	pInputData_ = _pInputData;
}


void mtgb::Input::CheckValidPadID()
{
	for (int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		DWORD result = XInputGetState(i, &pInputData_->gamePadStateCurrent_[i]);

		if (result == ERROR_SUCCESS)
		{
			pInputData_->activeGamePadID.insert(std::make_pair(PadIDState::UNASSIGNED, i));
		}
		else
		{
			pInputData_->activeGamePadID.insert(std::make_pair(PadIDState::INVALID, i));
		}
	}
}

void mtgb::Input::ChangeJoystickDevice(ComPtr<IDirectInputDevice8> _pJoystickDevice)
{
	pJoystickDevice_ = _pJoystickDevice;
}

/// <summary>
/// ジョイスティックが接続されている場合、デバイスに割り当てる
/// </summary>
/// <param name="lpddi">デバイスの情報を持つインスタンス</param>
/// <param name="pvRef">EnumDevicesで渡した値</param>
/// <returns></returns>
BOOL CALLBACK EnumJoysticksCallback(const LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	auto& input = Game::System<Input>();

	// 割り当て予約がなかったらデバイスを作成しない
	if (input.IsNotSubscribed())
	{
		return DIENUM_STOP;
	}
	DeviceType devType = Input::GetDeviceType(*lpddi);

	int reservationIndex = input.FindReservationIndexForDevice(devType);

	if(reservationIndex < 0)
	{
		// デバイスを要求している予約はない
		return DIENUM_CONTINUE;
	}
	LPDIRECTINPUT8 pDirectInput = reinterpret_cast<LPDIRECTINPUT8>(pvRef);
	ComPtr<IDirectInputDevice8> pDevice = nullptr;

	// 割り当て予約があり、未割当なのでデバイス作成
	HRESULT hResult = pDirectInput->CreateDevice(lpddi->guidInstance, pDevice.GetAddressOf(), nullptr);
	massert(SUCCEEDED(hResult)
		&& "ジョイスティックのデバイスの作成に失敗 @EnumJoysticksCallback");
	
	if (Game::System<Input>().RegisterJoystickGuid(lpddi->guidInstance) == false)
	{
		//既に割り当て済みの為、他のデバイスの列挙に移す
		pDevice.Reset();
		return DIENUM_CONTINUE;
	}

	input.AssignJoystickToReservation(pDevice, static_cast<size_t>(reservationIndex), lpddi->guidInstance);
	LOGIMGUI_CAT("Input", "Assigned reservationIndex=%d", reservationIndex);

	// 予約がまだ残っているなら続行
	return input.IsNotSubscribed() ? DIENUM_STOP : DIENUM_CONTINUE;
}

void mtgb::Input::EnumJoystick()
{
	// 割り当て予約がなかったらデバイスを作成しない
	if (Game::System<Input>().IsNotSubscribed())
	{
		return;
	}
	pDirectInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, pDirectInput_.Get(), DIEDFL_ATTACHEDONLY);
}

void mtgb::Input::RequestJoystickDevice(const JoystickReservation& _reservation)
{
	requestedJoystickDevices_.push_back(_reservation);
	StartEnumTimer();
}

void mtgb::Input::RequestJoystickDevice(JoystickReservation&& _reservation)
{
	requestedJoystickDevices_.push_back(std::move(_reservation));
	StartEnumTimer();
}

void mtgb::Input::AssignJoystickToReservation(ComPtr<IDirectInputDevice8> _pJoystickDevice, size_t _reservationIndex, GUID _guid)
{
	if (_reservationIndex >= requestedJoystickDevices_.size())
		return;

	// 予約をムーブ
	JoystickReservation reservation = std::move(requestedJoystickDevices_[_reservationIndex]);
	requestedJoystickDevices_.erase(requestedJoystickDevices_.begin() + _reservationIndex);

	// 協調レベル等設定
	_pJoystickDevice->SetCooperativeLevel(reservation.hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	//_pJoystickDevice->SetCooperativeLevel(reservation.hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	_pJoystickDevice->SetDataFormat(&c_dfDIJoystick);
	SetProperty(_pJoystickDevice, reservation.config);

	//デバイスからJoystickContext構築
	GUID guid = GetDeviceGuid(_pJoystickDevice);
	auto [itr, inserted] = joystickContext_.emplace(guid,_pJoystickDevice);

	if (reservation.onAssign)
		reservation.onAssign(itr->second.device, guid);

	//SetAcquireInterval(guid, itr->second.device);

}

void mtgb::Input::UnregisterJoystickGuid(GUID _guid)
{
	Timer::Remove(joystickContext_[_guid].timerHandle);
	joystickContext_.erase(_guid);
}

bool mtgb::Input::RegisterJoystickGuid(GUID _guid)
{
	return assignedJoystickGuids_.insert(_guid).second;
}

void mtgb::Input::SetAcquireInterval(GUID _guid, ComPtr<IDirectInputDevice8> _device)
{
	TimerHandle hTimer = Timer::AddInterval(acquireInterval, [&]() {AcquireJoystick(_device); });
	joystickContext_[_guid].timerHandle = hTimer;
}

bool mtgb::Input::IsNotSubscribed()
{
	return requestedJoystickDevices_.empty();
}

std::string mtgb::Input::GetDeviceName(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult)
		&& "デバイスの情報の取得に失敗しました　@Input::GetDeviceName");

	return std::string(deviceInstance.tszInstanceName);
}

std::string mtgb::Input::GetDeviceName(GUID _guid)
{
	if (joystickContext_.contains(_guid))
	{
		return GetDeviceName(joystickContext_[_guid].device);
	}
	return "None";
}

std::string mtgb::Input::GetDeviceProductName(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);
	massert(SUCCEEDED(hResult)
		&& "デバイスの情報の取得に失敗しました　@Input::GetDeviceName");

	return std::string(deviceInstance.tszProductName);
}

std::string mtgb::Input::GetDeviceProductName(GUID _guid)
{
	if (joystickContext_.contains(_guid))
	{
		return GetDeviceProductName(joystickContext_[_guid].device);
	}
	return "None";
}

std::string mtgb::Input::ConvertHResultToMessage(HRESULT hr) const
{
	switch (hr)
	{
	case DI_OK: return "取得";
	case S_FALSE:return "他アプリと共有";
	case DIERR_INPUTLOST:return "切断";
	case DIERR_NOTACQUIRED:return "デバイス未取得";
	case DIERR_OTHERAPPHASPRIO: return "他が優先権を所持";
	default:return"不明なエラー";
	}
}

DeviceType mtgb::Input::GetDeviceType(ComPtr<IDirectInputDevice8> _pInputDevice)
{
	DIDEVICEINSTANCE deviceInstance = {};
	deviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	HRESULT hResult = _pInputDevice->GetDeviceInfo(&deviceInstance);

	massert(SUCCEEDED(hResult)
		&& "デバイスの情報の取得に失敗しました　@Input::GetDeviceName");

	return Input::GetDeviceType(deviceInstance);

}

DeviceType mtgb::Input::GetDeviceType(const DIDEVICEINSTANCE& _inst)
{
	DeviceType deviceType = DeviceType::Unknown;

	//REF:https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee416610(v=vs.85)?devlangs=cpp&f1url=%3FappId%3DDev17IDEF1%26l%3DJA-JP%26k%3Dk(DINPUT%2FDIDEVICEINSTANCE)%3Bk(DIDEVICEINSTANCE)%3Bk(DevLang-C%2B%2B)%3Bk(TargetOS-Windows)%26rd%3Dtrue
	// 下位ビットでデバイスの大まかなタイプを判別
	// 上位ビットでデバイスのサブタイプも判別できるよ
	DWORD major = _inst.dwDevType & 0xFF;
	switch (major)
	{
	case DI8DEVTYPE_FLIGHT:
		deviceType = DeviceType::FlightStick;
		break;
	case DI8DEVTYPE_GAMEPAD:
	case DI8DEVTYPE_JOYSTICK:
	case DI8DEVTYPE_1STPERSON:
		deviceType = DeviceType::GamePad;
		break;
	default:
		deviceType = DeviceType::Unknown;
		break;
	}

	return deviceType;
}

HRESULT mtgb::Input::UpdateJoystickState(GUID guid)
{
	return E_NOTIMPL;
}


int mtgb::Input::FindReservationIndexForDevice(DeviceType _devType) const
{
	int firstUnknown = -1;
	for (size_t i = 0; i < requestedJoystickDevices_.size(); i++)
	{
		const auto& reservation = requestedJoystickDevices_[i];
		if (reservation.deviceType == _devType)
		{
			return static_cast<int>(i);
		}
		if (reservation.deviceType == DeviceType::Unknown && firstUnknown < 0)
		{
			firstUnknown = static_cast<int>(i);
		}
	}
		return firstUnknown;
}

const std::string mtgb::Input::GetJoystickStatusMessage(GUID guid) const
{
	const auto& itr = joystickContext_.find(guid);
	if (itr == joystickContext_.end())
	{
		return "未割当";
	}
	return ConvertHResultToMessage(itr->second.lastResult);
}

bool mtgb::Input::IsJoystickConnected(GUID guid) const
{
	const auto& itr = joystickContext_.find(guid);
	if (itr == joystickContext_.end())
	{
		return false;
	}
	switch (itr->second.lastResult)
	{
	case DI_OK:
	case S_FALSE:
		return true;
	default:
		return false;
	}
}

bool mtgb::Input::IsJoystickAssigned(GUID guid) const
{
	return (joystickContext_.find(guid) != joystickContext_.end());
}

void mtgb::Input::StartEnumTimer()
{
	if (!enumTimerHandle_ || IsNotSubscribed())
		return;

	enumTimerHandle_ = Timer::AddInterval(enumInterval_, [this]() {AutoEnum(); });
}

void mtgb::Input::StopEnumTimer()
{
	if (!enumTimerHandle_) return;
	Timer::Remove(enumTimerHandle_);
	enumTimerHandle_ = nullptr;
}

void mtgb::Input::AutoEnum()
{
	if (IsNotSubscribed())
	{
		StopEnumTimer();
		return;
	}
	EnumJoystick();
}

void mtgb::Input::SetProperty(ComPtr<IDirectInputDevice8> _pJoystickDevice, InputConfig _inputConfig)
{
	HRESULT hResult{};

#pragma region 軸モード設定

	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;

	//https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee416636(v=vs.85)
	//dwHowがDIPH_DEVICEの場合は0にしないといけない
	diprop.diph.dwObj = 0;

	//REL:前回のデバイスとの相対値を使用する
	//ABS:デバイス上の絶対値を使用する
	diprop.dwData = DIPROPAXISMODE_ABS;

	hResult = _pJoystickDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	massert(SUCCEEDED(hResult)
		&& "軸モードの設定に失敗");

#pragma endregion

#pragma region 値の範囲設定

	DIPROPRANGE diprg;
	diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;

	//X軸
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -_inputConfig.xRange;
	diprg.lMax = _inputConfig.xRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "値の範囲設定に失敗 @");

	//y軸
	diprg.diph.dwObj = DIJOFS_Y;
	diprg.lMin = -_inputConfig.yRange;
	diprg.lMax = _inputConfig.yRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "値の範囲設定に失敗 @");

	//z軸
	/*diprg.diph.dwObj = DIJOFS_Z;
	diprg.lMin = -_inputConfig.zRange;
	diprg.lMax = _inputConfig.zRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "値の範囲設定に失敗 @");*/

#pragma endregion
}

mtgb::JoystickContext::JoystickContext()
	:timerHandle{nullptr}
{}

mtgb::JoystickContext::~JoystickContext()
{
	if (timerHandle)
	{
		Timer::Remove(timerHandle);
	}
	device.Reset();
}

mtgb::JoystickContext::JoystickContext(ComPtr<IDirectInputDevice8> _device)
	:JoystickContext()
{
	device = _device;
	deviceType = Input::GetDeviceType(device);
}

mtgb::JoystickReservation::~JoystickReservation()
{
}
