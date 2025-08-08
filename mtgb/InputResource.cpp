#include "InputResource.h"
#include "InputData.h"
#include "ReleaseUtility.h"
#include "WindowContextUtil.h"
using namespace mtgb;

mtgb::InputResource::InputResource()
	:pInputData_{nullptr},pKeyDevice_{nullptr},pMouseDevice_{nullptr}
{
}

mtgb::InputResource::~InputResource()
{
	SAFE_DELETE(pInputData_);
	pKeyDevice_.Reset();
	pMouseDevice_.Reset();
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
}

void InputResource::SetResource()
{
	Input& input = Game::System<Input>();
	input.ChangeKeyDevice(pKeyDevice_);
	input.ChangeMouseDevice(pMouseDevice_);
	input.ChangeInputData(pInputData_);
}

InputResource* mtgb::InputResource::Clone() const
{
	return new InputResource(*this);
}

