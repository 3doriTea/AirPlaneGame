#pragma once
#include "WindowContextResource.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include "WindowContext.h"
#include "WindowManager.h"

#include <Windows.h>

typedef struct HWND__* HWND;
typedef struct tagWNDCLASSEXA WNDCLASSEX;
namespace mtgb
{
	class WindowResource : public WindowContextResource
	{
	public:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		template<typename... Args>
		void Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext);
		void SetResource() override;
		
		HWND hWnd_;
		bool isActive_;
		int outputIndex_;
		static int outputCount;
	};
	
	template<typename ...Args>
	inline void WindowResource::Initialize(std::tuple<Args*...>& _resourceTuple, WindowContext _windowContext)
	{
		// WindowManagerからWindowConfigを取得してウィンドウを作成
		WindowConfig config = Game::System<WindowManager>().GetWindowConfig(_windowContext);
		hWnd_ = Game::System<WindowManager>().CreateWindowContext(config);
		
		isActive_ = true;
		outputIndex_ = 0;
	}
}