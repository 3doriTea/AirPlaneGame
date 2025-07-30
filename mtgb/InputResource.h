#pragma once
#include "WindowContextResource.h"
#include "MTAssert.h"
#include "Game.h"
#include "ISystem.h"
#include "Input.h"
#include <dinput.h>

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
		template<typename... Args>
		void Initialize(std::tuple<Args*...>& _resourceTuple, HWND _hWnd);
		void SetResource() override;
	private:
		LPDIRECTINPUTDEVICE8 pKeyDevice_;    // キーデバイス
		LPDIRECTINPUTDEVICE8 pMouseDevice_;  // マウスデバイス
		InputData* inputData_;
	};

	template<typename... Args>
	inline void InputResource::Initialize(std::tuple<Args*...>& _resourceTuple, HWND _hWnd)
	{
		Game::System<Input>().CreateKeyDevice(_hWnd, &pKeyDevice_);
		Game::System<Input>().CreateMouseDevice(_hWnd, &pMouseDevice_);
	}
}