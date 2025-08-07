#include "Input.h"
#include "IncludingWindows.h"
#include "IncludingInput.h"
#include "InputData.h"
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "MainWindow.h"
#include "DoubleWindow.h"
#include "InputResource.h"

namespace
{
	static const size_t KEY_BUFFER_SIZE{ 256 };
}

mtgb::Input::Input() :
	pInputData_  { nullptr },
	pDirectInput_{ nullptr },
	pKeyDevice_  { nullptr },
	pMouseDevice_{ nullptr },
	pXInputData_ { nullptr }
{
}

mtgb::Input::~Input()
{
	pMouseDevice_.Reset();
	pKeyDevice_.Reset();
	pDirectInput_.Reset();
	SAFE_DELETE(pXInputData_);
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

	//#pragma region キーボード
	//// キーデバイス作成
	//hResult = pDirectInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr);

	//massert(SUCCEEDED(hResult)  // キーボードデバイスの作成に成功
	//	&& "キーボードデバイスの作成に失敗 @Input::Initialize");

	//// キーボード用にフォーマット
	//hResult = pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);

	//massert(SUCCEEDED(hResult)  // キーボードフォーマットに成功
	//	&& "キーボードフォーマットに失敗 @Input::Initialize");

	//// キーボードのアプリ間共有レベルを設定
	////  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	//hResult = pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//
	//massert(SUCCEEDED(hResult)  // キーボードアプリ間共有レベル設定に成功
	//	&& "キーボードアプリ間共有レベル設定に失敗 @Input::Initialize");
	//#pragma endregion

	//#pragma region マウス
	//// マウスデバイス作成
	//hResult = pDirectInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr);

	//massert(SUCCEEDED(hResult)  // マウスデバイスの作成に成功
	//	&& "マウスデバイスの作成に失敗 @Input::Initialize");

	//// マウス用にフォーマット
	//hResult = pMouseDevice_->SetDataFormat(&c_dfDIMouse);

	//massert(SUCCEEDED(hResult)  // マウスフォーマットに成功
	//	&& "マウスフォーマットに失敗 @Input::Initialize");

	//// マウスのアプリ間共有レベルの設定
	//hResult = pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	//massert(SUCCEEDED(hResult)  // マウスアプリ間共有レベル設定に成功
	//	&& "マウスアプリ間共有レベル設定に失敗 @Input::Initialize");
	//#pragma endregion
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
