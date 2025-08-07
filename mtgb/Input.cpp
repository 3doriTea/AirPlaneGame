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

	// DirectInput8�̃f�o�C�X�쐬
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(pDirectInput_.GetAddressOf()),
		nullptr);

	massert(SUCCEEDED(hResult)  // DirectInput8�̃f�o�C�X�쐬�ɐ���
		&& "DirectInput8�̃f�o�C�X�쐬�Ɏ��s @Input::Initialize");

	//#pragma region �L�[�{�[�h
	//// �L�[�f�o�C�X�쐬
	//hResult = pDirectInput_->CreateDevice(GUID_SysKeyboard, &pKeyDevice_, nullptr);

	//massert(SUCCEEDED(hResult)  // �L�[�{�[�h�f�o�C�X�̍쐬�ɐ���
	//	&& "�L�[�{�[�h�f�o�C�X�̍쐬�Ɏ��s @Input::Initialize");

	//// �L�[�{�[�h�p�Ƀt�H�[�}�b�g
	//hResult = pKeyDevice_->SetDataFormat(&c_dfDIKeyboard);

	//massert(SUCCEEDED(hResult)  // �L�[�{�[�h�t�H�[�}�b�g�ɐ���
	//	&& "�L�[�{�[�h�t�H�[�}�b�g�Ɏ��s @Input::Initialize");

	//// �L�[�{�[�h�̃A�v���ԋ��L���x����ݒ�
	////  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	//hResult = pKeyDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//
	//massert(SUCCEEDED(hResult)  // �L�[�{�[�h�A�v���ԋ��L���x���ݒ�ɐ���
	//	&& "�L�[�{�[�h�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::Initialize");
	//#pragma endregion

	//#pragma region �}�E�X
	//// �}�E�X�f�o�C�X�쐬
	//hResult = pDirectInput_->CreateDevice(GUID_SysMouse, &pMouseDevice_, nullptr);

	//massert(SUCCEEDED(hResult)  // �}�E�X�f�o�C�X�̍쐬�ɐ���
	//	&& "�}�E�X�f�o�C�X�̍쐬�Ɏ��s @Input::Initialize");

	//// �}�E�X�p�Ƀt�H�[�}�b�g
	//hResult = pMouseDevice_->SetDataFormat(&c_dfDIMouse);

	//massert(SUCCEEDED(hResult)  // �}�E�X�t�H�[�}�b�g�ɐ���
	//	&& "�}�E�X�t�H�[�}�b�g�Ɏ��s @Input::Initialize");

	//// �}�E�X�̃A�v���ԋ��L���x���̐ݒ�
	//hResult = pMouseDevice_->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	//massert(SUCCEEDED(hResult)  // �}�E�X�A�v���ԋ��L���x���ݒ�ɐ���
	//	&& "�}�E�X�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::Initialize");
	//#pragma endregion
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

	pInputData_->keyStatePrevious_ = pInputData_->keyStateCurrent_;
	pKeyDevice_->GetDeviceState(KEY_BUFFER_SIZE, keyBuffer);

	// TODO: for�ŉ񂷂̂̓R�X�p�悭�Ȃ�
	for (int i = 0; i < KEY_BUFFER_SIZE; i++)
	{
		pInputData_->keyStateCurrent_[i] = keyBuffer[i];
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
		&pInputData_->mouseStatePrevious_,
		&pInputData_->mouseStateCurrent_,
		sizeof(DIMOUSESTATE));

	hResult = pMouseDevice_->GetDeviceState(
		sizeof(DIMOUSESTATE),
		&pInputData_->mouseStateCurrent_);

	massert(SUCCEEDED(hResult)  // �}�E�X����̎擾�ɐ���
		&& "�}�E�X����̎擾�Ɏ��s @Input::Update");
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
	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�f�o�C�X�̍쐬�ɐ���
		&& "�L�[�{�[�h�f�o�C�X�̍쐬�Ɏ��s @Input::CreateKeyDevice");

	// �L�[�{�[�h�p�Ƀt�H�[�}�b�g
	hResult = (*_ppKeyDevice)->SetDataFormat(&c_dfDIKeyboard);

	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�t�H�[�}�b�g�ɐ���
		&& "�L�[�{�[�h�t�H�[�}�b�g�Ɏ��s @Input::CreateDevice");

	// �L�[�{�[�h�̃A�v���ԋ��L���x����ݒ�
	//  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	//hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//��A�N�e�B�u�ȃA�v�������͂��󂯕t����
	hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�A�v���ԋ��L���x���ݒ�ɐ���
		&& "�L�[�{�[�h�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::CreateDevice");
}

void mtgb::Input::CreateMouseDevice(HWND _hWnd, LPDIRECTINPUTDEVICE8* _ppMouseDevice)
{
	HRESULT hResult{};

	hResult = pDirectInput_->CreateDevice(GUID_SysMouse, _ppMouseDevice, nullptr);
	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�f�o�C�X�̍쐬�ɐ���
		&& "�}�E�X�f�o�C�X�̍쐬�Ɏ��s @Input::CreateMouseDevice");

	// �}�E�X�p�Ƀt�H�[�}�b�g
	hResult = (*_ppMouseDevice)->SetDataFormat(&c_dfDIMouse);

	massert(SUCCEEDED(hResult)  // �}�E�X�t�H�[�}�b�g�ɐ���
		&& "�}�E�X�t�H�[�}�b�g�Ɏ��s @Input::CreateMouseDevice");

	// �}�E�X�̃A�v���ԋ��L���x���̐ݒ�
	//hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//��A�N�e�B�u�ȃA�v�������͂��󂯕t����
	hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	massert(SUCCEEDED(hResult)  // �}�E�X�A�v���ԋ��L���x���ݒ�ɐ���
		&& "�}�E�X�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::CreateMouseDevice");
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
