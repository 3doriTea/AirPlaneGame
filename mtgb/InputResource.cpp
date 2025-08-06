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
	

	Game::System<Input>().RequestJoystickDevice(hWnd, pInputData_->config_, &pJoystickDevice_);

	Game::System<Input>().EnumJoystick();
}

void mtgb::InputResource::Update()
{
	*pProxy_ = pInputData_->joyStateCurrent_;
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

