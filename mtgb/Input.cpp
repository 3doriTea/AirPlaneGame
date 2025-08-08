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
	case DI_OK:  //�擾�ł���
	case S_FALSE://���̃A�v���������擾���Ă���
		break;
	case DIERR_OTHERAPPHASPRIO://���̃A�v�����D�挠�������Ă���
		return;
	case DIERR_INVALIDPARAM:
	case DIERR_NOTINITIALIZED:
		massert(SUCCEEDED(hResult)
			&& "�W���C�X�e�B�b�N����̋��擾�̍ۂɃG���[���N����܂��� @Input::Update");
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
		&& "�f�o�C�X�̏��̎擾�Ɏ��s���܂��� @Input::Update");
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

	// DirectInput8�̃f�o�C�X�쐬
	hResult = DirectInput8Create(
		GetModuleHandle(nullptr),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		reinterpret_cast<void**>(pDirectInput_.GetAddressOf()),
		nullptr);

	massert(SUCCEEDED(hResult)  // DirectInput8�̃f�o�C�X�쐬�ɐ���
		&& "DirectInput8�̃f�o�C�X�쐬�Ɏ��s @Input::Initialize");

}

void mtgb::Input::Update()
{
	
	static HRESULT hResult{};

#pragma region �L�[�{�[�h
	UpdateKeyDevice();
#pragma endregion

#pragma region �}�E�X
	UodateMouseDevice();
#pragma endregion

#pragma region �W���C�X�e�B�b�N
	
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
}

void mtgb::Input::UodateMouseDevice()
{
	static HRESULT hResult{};

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
	case DIERR_INPUTLOST://���̓��X�g�A�ꎞ�I�ȃA�N�Z�X�s��
	case DIERR_NOTACQUIRED://���擾
		AcquireJoystick(joystickContext_[currJoystickGuid_].device);
		return;
	default://���炩�̎��s
	{
		//�f�o�C�X�����蓖�čς݃��X�g���珜�O
		UnregisterJoystickGuid(GetDeviceGuid(joystickContext_[currJoystickGuid_].device));
		return;
	}
	/*massert(false
		&& "�f�o�C�X�̏�Ԃ̎擾�̍ۂɃG���[���N����܂��� @Input::Update");*/
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
	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�f�o�C�X�̍쐬�ɐ���
		&& "�L�[�{�[�h�f�o�C�X�̍쐬�Ɏ��s @Input::CreateKeyDevice");

	// �L�[�{�[�h�p�Ƀt�H�[�}�b�g
	hResult = (*_ppKeyDevice)->SetDataFormat(&c_dfDIKeyboard);

	massert(SUCCEEDED(hResult)  // �L�[�{�[�h�t�H�[�}�b�g�ɐ���
		&& "�L�[�{�[�h�t�H�[�}�b�g�Ɏ��s @Input::CreateDevice");

	// �L�[�{�[�h�̃A�v���ԋ��L���x����ݒ�
	//  REF: https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee417921(v=vs.85)
	hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//��A�N�e�B�u�ȃA�v�������͂��󂯕t����
	//hResult = (*_ppKeyDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

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
	hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	//��A�N�e�B�u�ȃA�v�������͂��󂯕t����
	//hResult = (*_ppMouseDevice)->SetCooperativeLevel(_hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);

	massert(SUCCEEDED(hResult)  // �}�E�X�A�v���ԋ��L���x���ݒ�ɐ���
		&& "�}�E�X�A�v���ԋ��L���x���ݒ�Ɏ��s @Input::CreateMouseDevice");
}

void mtgb::Input::ChangeKeyDevice(ComPtr<IDirectInputDevice8> _pKeyDevice)
{
	pKeyDevice_ = _pKeyDevice;
}

void mtgb::Input::SetJoystickGuid(GUID _guid)
{
	massert(assignedJoystickGuids_.contains(_guid)
		&& "������GUID���n����܂��� @Input::SetJoystickGuid");
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
/// �W���C�X�e�B�b�N���ڑ�����Ă���ꍇ�A�f�o�C�X�Ɋ��蓖�Ă�
/// </summary>
/// <param name="lpddi">�f�o�C�X�̏������C���X�^���X</param>
/// <param name="pvRef">EnumDevices�œn�����l</param>
/// <returns></returns>
BOOL CALLBACK EnumJoysticksCallback(const LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
	LPDIRECTINPUT8 pDirectInput = reinterpret_cast<LPDIRECTINPUT8>(pvRef);
	LPDIRECTINPUTDEVICE8A pJoyStick = nullptr;
	
	if (!Game::System<Input>().RegisterJoystickGuid(lpddi->guidInstance))
	{
		//���Ɋ��蓖�čςׁ݂̈A���̃f�o�C�X�̗񋓂Ɉڂ�
		return DIENUM_CONTINUE;
	}
	HRESULT hResult = pDirectInput->CreateDevice(lpddi->guidInstance, &pJoyStick, nullptr);
	massert(SUCCEEDED(hResult)
		&& "�W���C�X�e�B�b�N�̃f�o�C�X�̍쐬�Ɏ��s @EnumJoysticksCallback");
	


	Game::System<Input>().AssignJoystick(pJoyStick);
	
	//���̃f�o�C�X���񋓂������Ă��������Ƃ����w��
	return DIENUM_CONTINUE;
}

void mtgb::Input::EnumJoystick()
{
	// ���蓖�ė\�񂪂Ȃ�������f�o�C�X���쐬���Ȃ�
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

	//�f�o�C�X����JoystickContext�\�z
	const auto& pair = joystickContext_.emplace(GetDeviceGuid(_pJoystickDevice),_pJoystickDevice);
	if (!pair.second)
	{
		//���łɓo�^�ς݂̃f�o�C�X
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
	case DI_OK: return "�擾";
	case S_FALSE:return "���A�v���Ƌ��L";
	case DIERR_INPUTLOST:return "�ؒf";
	case DIERR_NOTACQUIRED:return "�f�o�C�X���擾";
	case DIERR_OTHERAPPHASPRIO: return "�����D�挠������";
	default:return"�s���ȃG���[";
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
		return "������";
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

#pragma region �����[�h�ݒ�

	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwHow = DIPH_DEVICE;

	//https://learn.microsoft.com/ja-jp/previous-versions/windows/desktop/ee416636(v=vs.85)
	//dwHow��DIPH_DEVICE�̏ꍇ��0�ɂ��Ȃ��Ƃ����Ȃ�
	diprop.diph.dwObj = 0;

	//REL:�O��̃f�o�C�X�Ƃ̑��Βl���g�p����
	//ABS:�f�o�C�X��̐�Βl���g�p����
	diprop.dwData = DIPROPAXISMODE_ABS;

	hResult = _pJoystickDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	massert(SUCCEEDED(hResult)
		&& "�����[�h�̐ݒ�Ɏ��s");

#pragma endregion

#pragma region �l�͈̔͐ݒ�

	DIPROPRANGE diprg;
	diprg;
	diprg.diph.dwSize = sizeof(diprg);
	diprg.diph.dwHeaderSize = sizeof(diprg.diph);
	diprg.diph.dwHow = DIPH_BYOFFSET;

	//X��
	diprg.diph.dwObj = DIJOFS_X;
	diprg.lMin = -_inputConfig.xRange;
	diprg.lMax = _inputConfig.xRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "�l�͈̔͐ݒ�Ɏ��s @");

	//y��
	diprg.diph.dwObj = DIJOFS_Y;
	diprg.lMin = -_inputConfig.yRange;
	diprg.lMax = _inputConfig.yRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "�l�͈̔͐ݒ�Ɏ��s @");

	//z��
	/*diprg.diph.dwObj = DIJOFS_Z;
	diprg.lMin = -_inputConfig.zRange;
	diprg.lMax = _inputConfig.zRange;

	hResult = _pJoystickDevice->SetProperty(DIPROP_RANGE, &diprg.diph);
	massert(SUCCEEDED(hResult)
		&& "�l�͈̔͐ݒ�Ɏ��s @");*/

#pragma endregion
}

mtgb::JoystickContext::JoystickContext(IDirectInputDevice8* _device)
{
	device.Attach(_device);
}
