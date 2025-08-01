#include "InputResource.h"
#include "InputData.h"
using namespace mtgb;

mtgb::InputResource::InputResource()
	:pInputData_{nullptr},pKeyDevice_{nullptr},pMouseDevice_{nullptr}
{
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
	HWND hWnd = Game::System<WindowContextResourceManager>().GetHWND(_windowContext);

	Game::System<Input>().CreateKeyDevice(hWnd, &pKeyDevice_);
	Game::System<Input>().CreateMouseDevice(hWnd, &pMouseDevice_);
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

