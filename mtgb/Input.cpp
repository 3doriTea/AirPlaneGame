#include "Input.h"
#include <Windows.h>
#include <dinput.h>
#include "InputData.h"
#include "ReleaseUtility.h"
#include "MTAssert.h"
#include "MainWindow.h"


namespace
{
	static const size_t KEY_BUFFER_SIZE{ 256 };
}

mtgb::Input::Input() :
	pDirectInput_{ nullptr },
	pKeyDevice_{ nullptr },
	pMouseDevice_{ nullptr }
{
}

mtgb::Input::~Input()
{
	SAFE_RELEASE(pMouseDevice_);
	SAFE_RELEASE(pKeyDevice_);
	SAFE_RELEASE(pDirectInput_);
}

void mtgb::Input::Initialize()
{
	HRESULT hResult{};

	const HWND hWnd{ Game::System<MainWindow>().GetHWND() };

	// DirectInput8のデバイス作成
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(&pDirectInput_),
		nullptr);

	massert(SUCCEEDED(hResult)  // DirectInput8のデバイス作成に成功
		&& "DirectInput8のデバイス作成に失敗 @Input::Initialize");

	#pragma region キーボード
	// キーデバイス作成
	hResult = pDirectInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr);

	massert(SUCCEEDED(hResult)  // キーボードデバイスの作成に成功
		&& "キーボードデバイスの作成に失敗 @Input::Initialize");

	// キーボード用にフォーマット
	hResult = pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);

	massert(SUCCEEDED(hResult)  // キーボードフォーマットに成功
		&& "キーボードフォーマットに失敗 @Input::Initialize");

	// キーボードのアプリ間共有レベルを設定
	//  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	hResult = pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	
	massert(SUCCEEDED(hResult)  // キーボードアプリ間共有レベル設定に成功
		&& "キーボードアプリ間共有レベル設定に失敗 @Input::Initialize");
	#pragma endregion

	#pragma region マウス
	// マウスデバイス作成
	hResult = pDirectInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr);

	massert(SUCCEEDED(hResult)  // マウスデバイスの作成に成功
		&& "マウスデバイスの作成に失敗 @Input::Initialize");

	// マウス用にフォーマット
	hResult = pMouseDevice_->SetDataFormat(&c_dfDIMouse);

	massert(SUCCEEDED(hResult)  // マウスフォーマットに成功
		&& "マウスフォーマットに失敗 @Input::Initialize");

	// マウスのアプリ間共有レベルの設定
	hResult = pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	massert(SUCCEEDED(hResult)  // マウスアプリ間共有レベル設定に成功
		&& "マウスアプリ間共有レベル設定に失敗 @Input::Initialize");
	#pragma endregion
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

	InputData::keyStatePrevious_ = InputData::keyStateCurrent_;
	pKeyDevice_->GetDeviceState(KEY_BUFFER_SIZE, keyBuffer);

	// TODO: forで回すのはコスパよくない
	for (int i = 0; i < KEY_BUFFER_SIZE; i++)
	{
		InputData::keyStateCurrent_[i] = keyBuffer[i];
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
		&InputData::mouseStatePrevious_,
		&InputData::mouseStateCurrent_,
		sizeof(_DIMOUSESTATE));

	pMouseDevice_->GetDeviceState(
		sizeof(_DIMOUSESTATE),
		&InputData::mouseStateCurrent_);
#pragma endregion
}

void mtgb::Input::UpdateMousePositionData(
	const int32_t _x,
	const int32_t _y)
{
	InputData::mousePosition_.x = _x;
	InputData::mousePosition_.y = _y;
}
