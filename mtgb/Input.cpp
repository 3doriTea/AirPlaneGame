#include "Input.h"
#include "IncludingWindows.h"
#include "IncludingInput.h"
#include "InputData.h"
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "MainWindow.h"
#include "DoubleWindow.h"
#include "InputResource.h"
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
	pDirectInput_{ nullptr },
	pKeyDevice_{ nullptr },
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
	UodateMouseDevice();
#pragma endregion

#pragma region ジョイスティック
	
	/*if (pJoystickDevice_ == nullptr)
	{
		return;
	}*/
	UpdateJoystickDevice();
	
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

void mtgb::Input::UodateMouseDevice()
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
}

void mtgb::Input::UpdateJoystickDevice()
{
	static HRESULT hResult{};
	
	if (joystickContext_.empty()) return;

	memcpy(
		&pInputData_->joyStatePrevious_,
		&pInputData_->joyStateCurrent_,
		sizeof(DIJOYSTATE));


	hResult = joystickContext_[currJoystickGuid_].device->GetDeviceState(sizeof(DIJOYSTATE), &pInputData_->joyStateCurrent_);
	joystickContext_[currJoystickGuid_].lastResult = hResult;
	switch (hResult)
	{
	case DI_OK:
		LOGF("OK\n");
		break;
	case DIERR_INPUTLOST://入力ロスト、一時的なアクセス不可
	case DIERR_NOTACQUIRED://未取得
		AcquireJoystick(joystickContext_[currJoystickGuid_].device);
		return;
	default://何らかの失敗
	{
		//デバイスを割り当て済みリストから除外
		UnregisterJoystickGuid(GetDeviceGuid(joystickContext_[currJoystickGuid_].device));
		return;
	}
	/*massert(false
		&& "デバイスの状態の取得の際にエラーが起こりました @Input::Update");*/
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
	pInputData_->mousePosition_.x = _x;
	pInputData_->mousePosition_.y = _y;
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
	hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//非アクティブなアプリも入力を受け付ける
	//hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

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
	hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//非アクティブなアプリも入力を受け付ける
	//hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(SUCCEEDED(hResult)  // マウスアプリ間共有レベル設定に成功
		&& "マウスアプリ間共有レベル設定に失敗 @Input::CreateMouseDevice");
}

void mtgb::Input::ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice)
{
	pKeyDevice_ = _pKeyDevice;
}

void mtgb::Input::SetJoystickGuid(GUID _guid)
{
	massert(assignedJoystickGuids_.contains(_guid)
		&& "無効なGUIDが渡されました @Input::SetJoystickGuid");
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
	LPDIRECTINPUT8 pDirectInput = reinterpret_cast<LPDIRECTINPUT8>(pvRef);
	LPDIRECTINPUTDEVICE8A pJoyStick = nullptr;
	
	if (!Game::System<Input>().RegisterJoystickGuid(lpddi->guidInstance))
	{
		//既に割り当て済みの為、他のデバイスの列挙に移す
		return DIENUM_CONTINUE;
	}
	HRESULT hResult = pDirectInput->CreateDevice(lpddi->guidInstance, &pJoyStick, nullptr);
	massert(SUCCEEDED(hResult)
		&& "ジョイスティックのデバイスの作成に失敗 @EnumJoysticksCallback");
	


	Game::System<Input>().AssignJoystick(pJoyStick);
	
	//他のデバイスも列挙し続けてくださいという指示
	return DIENUM_CONTINUE;
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

//void mtgb::Input::RequestJoystickDevice(HWND _hWnd, InputConfig _inputConfig, ComPtr<IDirectInputDevice8>* _pJoystickDevice)
//{
//	requestedJoystickDevices_.push_back(std::make_tuple(_hWnd, _inputConfig, _pJoystickDevice));
//}

void mtgb::Input::RequestJoystickDevice(JoystickReservation* _reservation)
{
	requestedJoystickDevices_.push_back(_reservation);
}

void mtgb::Input::AssignJoystick(IDirectInputDevice8* _pJoystickDevice)
{
	auto& front = requestedJoystickDevices_.front();
	HWND hWnd = front->hWnd;
	//_pJoystickDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
	_pJoystickDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	_pJoystickDevice->SetDataFormat(&c_dfDIJoystick);
	SetProperty(_pJoystickDevice, front->config);

	//デバイスからJoystickContext構築
	const auto& pair = joystickContext_.emplace(GetDeviceGuid(_pJoystickDevice),_pJoystickDevice);
	if (!pair.second)
	{
		//すでに登録済みのデバイス
		return;
	}
	front->onAssign(pair.first->second.device, GetDeviceGuid(_pJoystickDevice));

	requestedJoystickDevices_.erase(requestedJoystickDevices_.begin());
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

HRESULT mtgb::Input::UpdateJoystickState(GUID guid)
{
	return E_NOTIMPL;
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

mtgb::JoystickContext::JoystickContext(IDirectInputDevice8* _device)
{
	device.Attach(_device);
}
