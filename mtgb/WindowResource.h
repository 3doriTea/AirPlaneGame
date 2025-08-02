#pragma once
#include "WindowContextResource.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include "WindowContext.h"
#include "WindowManager.h"
#include "WindowContextResourceManager.h"
#include <Windows.h>

#include <map>
#include <typeindex>

namespace mtgb
{

	class WindowResource : public WindowContextResource
	{
	public:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
		
		HWND hWnd_;
		bool isActive_;
		int outputIndex_;
		static int outputCount;

		
		WindowResource(const WindowResource& other);
		// WindowContextResource ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		WindowResource* Clone() const override;
		WindowResource();
		~WindowResource();
	};
}
