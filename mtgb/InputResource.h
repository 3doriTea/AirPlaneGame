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
		InputResource(const InputResource& other);
		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
		const InputData* GetInput(){ return pInputData_; }
		InputData* pInputData_;
	private:
		LPDIRECTINPUTDEVICE8 pKeyDevice_;    // キーデバイス
		LPDIRECTINPUTDEVICE8 pMouseDevice_;

		// WindowContextResource を介して継承されました
		InputResource* Clone() const override;
		// マウスデバイス
	};
}