#include "InputResource.h"
#include "InputData.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
#include "IncludingInput.h"
#include "JoystickProxy.h"
using namespace mtgb;
namespace
{
	static int id = 0;
}
mtgb::InputResource::InputResource()
	:pInputData_{nullptr},pKeyDevice_{nullptr},pMouseDevice_{nullptr},pProxy_{nullptr}
{
	
}

mtgb::InputResource::~InputResource()
{
	SAFE_DELETE(pInputData_);
	pKeyDevice_.Reset();
	pMouseDevice_.Reset();
	pJoystickDevice_.Reset();
}

mtgb::InputResource::InputResource(const InputResource& other)
	:WindowContextResource(other)
{
	if (other.pInputData_)
	{
		pInputData_ = new InputData(*other.pInputData_);
	}
	else
	{
		pInputData_ = nullptr;
	}
	pKeyDevice_ = nullptr;
	pMouseDevice_ = nullptr;
}

void mtgb::InputResource::Initialize(WindowContext _windowContext)
{
	HWND hWnd = WinCtxRes::GetHWND(_windowContext);

	IDirectInputDevice8* pRawKeyDevice = nullptr;
	Game::System<Input>().CreateKeyDevice(hWnd, &pRawKeyDevice);
	pKeyDevice_.Attach(pRawKeyDevice);

	IDirectInputDevice8* pRawMouseDevice = nullptr;
	Game::System<Input>().CreateMouseDevice(hWnd, &pRawMouseDevice);
	pMouseDevice_.Attach(pRawMouseDevice);

	pInputData_ = new InputData();
	pProxy_ = new JoystickProxy(pInputData_->joyStateCurrent_);
	pInputData_->config_.SetRange(1000);
	pInputData_->config_.SetDeadZone(0.1);
	pProxy_->SetDisplayName("proxy:"+ id++);
	
	
	reservation.config = pInputData_->config_;
	reservation.hWnd = hWnd;
	reservation.onAssign = [this](ComPtr<IDirectInputDevice8> device,GUID guid)
		{
			pJoystickDevice_ = device;
			assignedJoystickGuid_ = guid;
			isActive_ = true;
		};

	Game::System<Input>().RequestJoystickDevice(&reservation);

	Game::System<Input>().EnumJoystick();
}

void mtgb::InputResource::Update()
{
	if (!isActive_) return;
	Input& input = Game::System<Input>();
	

	pProxy_->UpdateFromInput(assignedJoystickGuid_);
	pProxy_->UpdateInputData(pInputData_->joyStateCurrent_);

	//���蓖�Ă��Ă��Ȃ��A�ؒf����Ă���ꍇ��return
	isActive_ = (input.IsJoystickAssigned(assignedJoystickGuid_) && input.IsJoystickConnected(assignedJoystickGuid_));
	
}

void InputResource::SetResource()
{
	Input& input = Game::System<Input>();
	input.ChangeKeyDevice(pKeyDevice_);
	input.ChangeMouseDevice(pMouseDevice_);
	input.ChangeJoystickDevice(pJoystickDevice_);
	input.ChangeInputData(pInputData_);
}

WindowContextResource* mtgb::InputResource::Clone() const
{
	return new InputResource(*this);
}

