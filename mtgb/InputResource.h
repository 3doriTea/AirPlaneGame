#pragma once
#include "WindowContextResource.h"
#include "MTAssert.h"
#include "Game.h"
#include "ISystem.h"
#include "Input.h"
#include "WindowContext.h"
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
		void Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext);
		void SetResource() override;
	private:
		LPDIRECTINPUTDEVICE8 pKeyDevice_;    // �L�[�f�o�C�X
		LPDIRECTINPUTDEVICE8 pMouseDevice_;  // �}�E�X�f�o�C�X
		InputData* inputData_;
	};

	template<typename... Args>
	inline void InputResource::Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext)
	{
		// WindowContextResourceManager����HWND���擾
		HWND hWnd = Game::System<WindowContextResourceManager>().GetHWND(_windowContext);
		
		Game::System<Input>().CreateKeyDevice(hWnd, &pKeyDevice_);
		Game::System<Input>().CreateMouseDevice(hWnd, &pMouseDevice_);
	}
}