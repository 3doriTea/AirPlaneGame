#pragma once
#include "WindowContextResource.h"
#include "WindowContextResourceManager.h"
#include "MTAssert.h"
#include "Game.h"
#include "ISystem.h"
#include "Input.h"
#include "WindowContext.h"
#include <dinput.h>
#include <map>
#include <typeindex>
#include "JoystickProxy.h"
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dInput8.lib")
#pragma comment(lib, "Xinput.lib")

typedef struct HWND__* HWND;

namespace mtgb
{

	class InputData;
	class InputResource : public WindowContextResource
	{
	public:
		InputResource();
		~InputResource();
		InputResource(const InputResource& other);
		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
		const InputData* GetInput(){ return pInputData_; }
		InputData* pInputData_;
	private:
		ComPtr<IDirectInputDevice8> pKeyDevice_;    // キーデバイス
		ComPtr<IDirectInputDevice8> pMouseDevice_;
		ComPtr<IDirectInputDevice8> pJoystickDevice_;
		JoystickProxy 

		// WindowContextResource を介して継承されました
		InputResource* Clone() const override;
		// マウスデバイス
	};
}