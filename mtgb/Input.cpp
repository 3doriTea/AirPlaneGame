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

	// DirectInput8�̃f�o�C�X�쐬
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(&pDirectInput_),
		nullptr);

	massert(SUCCEEDED(hResult)  // DirectInput8�̃f�o�C�X�쐬�ɐ���
		&& "DirectInput8�̃f�o�C�X�쐬�Ɏ��s @Input::Initialize");

	#pragma region �L�[�{�[�h
	// �L�[�f�o�C�X�쐬
	hResult = pDirectInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr);

	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�f�o�C�X�̍쐬�ɐ���
		&& "�L�[�{�[�h�f�o�C�X�̍쐬�Ɏ��s @Input::Initialize");

	// �L�[�{�[�h�p�Ƀt�H�[�}�b�g
	hResult = pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);

	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�t�H�[�}�b�g�ɐ���
		&& "�L�[�{�[�h�t�H�[�}�b�g�Ɏ��s @Input::Initialize");

	// �L�[�{�[�h�̃A�v���ԋ��L���x����ݒ�
	//  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	hResult = pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	
	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�A�v���ԋ��L���x���ݒ�ɐ���
		&& "�L�[�{�[�h�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::Initialize");
	#pragma endregion

	#pragma region �}�E�X
	// �}�E�X�f�o�C�X�쐬
	hResult = pDirectInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr);

	massert(SUCCEEDED(hResult)  // �}�E�X�f�o�C�X�̍쐬�ɐ���
		&& "�}�E�X�f�o�C�X�̍쐬�Ɏ��s @Input::Initialize");

	// �}�E�X�p�Ƀt�H�[�}�b�g
	hResult = pMouseDevice_->SetDataFormat(&c_dfDIMouse);

	massert(SUCCEEDED(hResult)  // �}�E�X�t�H�[�}�b�g�ɐ���
		&& "�}�E�X�t�H�[�}�b�g�Ɏ��s @Input::Initialize");

	// �}�E�X�̃A�v���ԋ��L���x���̐ݒ�
	hResult = pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	massert(SUCCEEDED(hResult)  // �}�E�X�A�v���ԋ��L���x���ݒ�ɐ���
		&& "�}�E�X�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::Initialize");
	#pragma endregion
}

void mtgb::Input::Update()
{
	static HRESULT hResult{};

#pragma region �L�[�{�[�h
	// �L�[�{�[�h����̋��Q�b�g
	hResult = pKeyDevice_->Acquire();

	//massert(SUCCEEDED(hResult)  // �L�[�{�[�h����̋��擾�ɐ���
	//	&& "�L�[�{�[�h����̋��擾�Ɏ��s @Input::Update");

	if (FAILED(hResult))
	{
		return;  // �L�[�{�[�h����̋��擾�Ɏ��s�����Ȃ��A
	}

	static BYTE keyBuffer[KEY_BUFFER_SIZE]{};  // �L�[��Ԏ擾�p�o�b�t�@

	InputData::keyStatePrevious_ = InputData::keyStateCurrent_;
	pKeyDevice_->GetDeviceState(KEY_BUFFER_SIZE, keyBuffer);

	// TODO: for�ŉ񂷂̂̓R�X�p�悭�Ȃ�
	for (int i = 0; i < KEY_BUFFER_SIZE; i++)
	{
		InputData::keyStateCurrent_[i] = keyBuffer[i];
	}
#pragma endregion

#pragma region �}�E�X
	// �}�E�X����̋����Q�b�g
	hResult = pMouseDevice_->Acquire();

	if (FAILED(hResult))
	{
		return;  // �}�E�X����̋��擾�Ɏ��s�����Ȃ��A
	}

	massert(SUCCEEDED(hResult)  // �}�E�X����̋��擾�ɐ���
		&& "�}�E�X����̋��擾�Ɏ��s @Input::Update");

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
