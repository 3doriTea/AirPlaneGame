#pragma once
#include <Windows.h>
#include "ISystem.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
namespace mtgb
{
	/// <summary>
			/// ウィンドウからのメッセージを受信してImGuiで入力やイベントを処理するためのコールバック関数
			/// </summary>
			/// <param name="hwnd">ウィンドウハンドル</param>
			/// <param name="msg">メッセージ</param>
			/// <param name="wParam">パラメータ</param>
			/// <param name="lParam">パラメータ</param>
			/// <returns></returns>
	//IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	class MTImGui final : public ISystem
	{
	public:
		~MTImGui();
		void Initialize() override;
		void Update() override;
		void BeginFrame();
		void EndFrame();
		void Release();
	private:
		//ImGuiIO io;
	};
}