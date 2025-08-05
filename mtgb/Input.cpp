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
namespace
{
	static const size_t KEY_BUFFER_SIZE{ 256 };
	LONG min = -1000;
	LONG max = 1000;
	LONG xMin{ min },
		 xMax{ max },
		 yMin{ min },
		 yMax{ max };
}

using namespace mtgb;

mtgb::Input::Input() :
	pDirectInput_{ nullptr },
	pKeyDevice_{ nullptr },
	pMouseDevice_{ nullptr }
{
}

mtgb::Input::~Input()
{
	pMouseDevice_.Reset();
	pKeyDevice_.Reset();
	pDirectInput_.Reset();
}

void mtgb::Input::Initialize()
{
	HRESULT hResult{};

	//const HWND hWnd{ Game::System<MainWindow>().GetHWND() };
	//const HWND hWnd{ Game::System<DoubleWindow>().GetFirstWindowHandle() };

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
#pragma endregion

#pragma region マウス
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

#pragma region ジョイスティック
	//ジョイスティック操作の許可をゲット
	hResult = pJoystickDevice_->Acquire();
	switch (hResult)
	{
	case S_FALSE://他のデバイスが許可を取得
	case DIERR_OTHERAPPHASPRIO://他のアプリが優先権を持っている
		return;
	default:
		massert(SUCCEEDED(hResult)
			&& "ジョイスティック操作の許可取得の際にエラーが起こりました @Input::Update");
	}

	memcpy(
		&pInputData_->joyStatePrevious_,
		&pInputData_->joyStateCurrent_,
		sizeof(DIJOYSTATE));
	hResult = pJoystickDevice_->GetDeviceState(sizeof(DIJOYSTATE),&pInputData_->joyStateCurrent_);
	
	switch (hResult)
	{
	case DI_OK:
		break;
	case DIERR_INPUTLOST://入力ロスト
		LPDIDEVICEINSTANCE pDeviceInstance;
		hResult = pJoystickDevice_->GetDeviceInfo(pDeviceInstance);
		massert(SUCCEEDED(hResult)
			&& "デバイスの情報の取得に失敗しました @Input::Update");

		//デバイスを割り当て済みリストから除外
		assignedJoystickGuids_.erase(pDeviceInstance->guidInstance);
		return;
	case DIERR_NOTACQUIRED://未取得
		return;
	default://何らかの失敗
		massert(false
			&& "デバイスの状態の取得の際にエラーが起こりました @Input::Update");
	}

#pragma endregion
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
	//hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//非アクティブなアプリも入力を受け付ける
	hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

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
	hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	massert(SUCCEEDED(hResult)  // マウスアプリ間共有レベル設定に成功
		&& "マウスアプリ間共有レベル設定に失敗 @Input::CreateMouseDevice");
}

void mtgb::Input::ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice)
{
	pKeyDevice_ = _pKeyDevice;
}

void mtgb::Input::ChangeMouseDevice(ComPtr<IDirectInputDevice8> _pMouseDevice)
{
	pMouseDevice_ = _pMouseDevice;
}

void mtgb::Input::ChangeInputData(InputData* _pInputData)
{
	pInputData_ = _pInputData;
}

void mtgb::Input::ChangeJoystickDevice(LPDIRECTINPUTDEVICE8 _pJoystickDevice)
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
	pDirectInput_->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, pDirectInput_, DIEDFL_ATTACHEDONLY);
}

void mtgb::Input::RequestJoystickDevice(HWND _hWnd, InputConfig _inputConfig, LPDIRECTINPUTDEVICE8 _pJoystickDevice)
{
	requestedJoystickDevices_.push_back(std::make_tuple(_hWnd,_inputConfig,_pJoystickDevice));
}

void mtgb::Input::AssignJoystick(LPDIRECTINPUTDEVICE8 _pJoystickDevice)
{
	HWND hWnd = std::get<HWND>(requestedJoystickDevices_.front());
	_pJoystickDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	
	//Joystick2は多機能なデバイスにも対応している
	_pJoystickDevice->SetDataFormat(&c_dfDIJoystick);
	//_pJoystickDevice->SetDataFormat(&c_dfDIJoystick2);
	std::get<LPDIRECTINPUTDEVICE8>(requestedJoystickDevices_.front()) = _pJoystickDevice;
}

bool mtgb::Input::RegisterJoystickGuid(GUID _guid)
{
	return assignedJoystickGuids_.insert(_guid).second;
}

bool mtgb::Input::IsNotSubscribed()
{
	return requestedJoystickDevices_.empty();
}

void mtgb::Input::SetProperty(LPDIRECTINPUTDEVICE8 _pJoystickDevice, InputConfig _inputConfig)
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
	diprg.lMin = _inputConfig.xRange;
	diprg.lMax = _inputConfig.xRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "値の範囲設定に失敗 @");

	//y軸
	diprg.diph.dwObj = DIJOFS_Y;
	diprg.lMin = _inputConfig.yRange;
	diprg.lMax = _inputConfig.yRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "値の範囲設定に失敗 @");

	//z軸
	diprg.diph.dwObj = DIJOFS_Z;
	diprg.lMin = _inputConfig.zRange;
	diprg.lMax = _inputConfig.zRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "値の範囲設定に失敗 @");

#pragma endregion
}
