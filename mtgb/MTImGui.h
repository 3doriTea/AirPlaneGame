#pragma once
#include <Windows.h>
#include "ISystem.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
namespace mtgb
{
	/// <summary>
			/// �E�B���h�E����̃��b�Z�[�W����M����ImGui�œ��͂�C�x���g���������邽�߂̃R�[���o�b�N�֐�
			/// </summary>
			/// <param name="hwnd">�E�B���h�E�n���h��</param>
			/// <param name="msg">���b�Z�[�W</param>
			/// <param name="wParam">�p�����[�^</param>
			/// <param name="lParam">�p�����[�^</param>
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