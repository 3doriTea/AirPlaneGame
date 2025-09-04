#pragma once
#include "WindowContextResource.h"
#include "Game.h"
#include "ISystem.h"
#include "DirectX11Manager.h"
#include "MTAssert.h"
#include "WindowContext.h"
#include "WindowManager.h"

#include "includingWindows.h"

#include <map>
#include <typeindex>

namespace mtgb
{

	class WindowResource : public WindowContextResource
	{
	public:
		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		LRESULT HandleWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		HWND GetHWND();

		void Initialize(WindowContext _windowContext) override;
		void SetResource() override;
		
		WindowResource(const WindowResource& other);
		// WindowContextResource を介して継承されました
		WindowResource* Clone() const override;
		WindowResource();
		~WindowResource();

		/// <summary>
		/// <para> リソースの初期化が完了したというフラグを trueにする </para>
		/// </summary>
		void MarkInitialized();
		void OnResize(WindowContext _windowContext, UINT _width, UINT _height) override;

		void SetFullScreen(bool _fullscreen);
		void SetFullScreen(bool _fullscreen, const RECT& _monitorRect);
		/// <summary>
		/// フルスクリーンにする前に情報を保持しておく
		/// </summary>
		void GetWindowInfo();
	private:
		WindowContext context_;
		HWND hWnd_;
		bool isActive_;

		// ウィンドウモード時の情報
		RECT windowedRect_;		//位置とサイズ
		LONG windowedStyle_;	// スタイル
		LONG windowedExStyle_;	// 拡張スタイル

		// 初期化が完了したか
		bool isInitialized_;

		// WindowContextResource を介して継承されました
		void Release() override;

	};
}
