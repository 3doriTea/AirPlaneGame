#pragma once
#include "IncludingWindows.h"
#include "ISystem.h"
#include <refl.hpp>
#include <type_traits>
#include <assert.h>
#include <typeinfo>
#include <string>
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include <DirectXMath.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;
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
		MTImGui();
		~MTImGui();
		void Initialize() override;
		void Update() override;
		void BeginFrame();

		void BeginImGuizmoFrame();
		void Begin(std::string str);
		/// <summary>
		/// ImGuizmo�E�B���h�E��`�悷�邽�߂�RTV���Z�b�g
		/// </summary>
		void SetImGuizmoRenderTargetView();
		void Draw();
		
		void EndFrame();
		void SetDrawList();
		void RenderGameView();
		bool IsUsingImGuizmo();
		bool IsOverImGuizmo();
		void End();
		void Release();
		bool DrawTransformGuizmo(float * _worldMat, const float * _viewMat, const float * _projMat, DirectX::XMFLOAT3 * _position, DirectX::XMVECTORF32 * _rotation, DirectX::XMFLOAT3 * _scale);
		
	private:
		UINT winWidth_, winHeight_;

		//ImGuiIO io;
		ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
		ComPtr<ID3D11ShaderResourceView> pSRV_;
		ComPtr<ID3D11Texture2D> pTexture_;
		ComPtr<ID3D11Texture2D> pDepthStencil_;
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_;
	};
}