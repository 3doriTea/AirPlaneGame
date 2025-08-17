#pragma once
#include "IncludingWindows.h"
#include "ISystem.h"

#include <type_traits>
#include <assert.h>
#include <typeinfo>
#include <string>
#include "../ImGui/imgui.h"
#include "../ImGui\ImGuizmo.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include <DirectXMath.h>
#include <wrl/client.h>
#include "ImGuiShowable.h"
#include "Handlers.h"
#include <d3d11.h>
#include "ShowType.h"
#include "Matrix4x4.h"

using Microsoft::WRL::ComPtr;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;
struct ID3D11DepthStencilView;

namespace mtgb
{
	class GameObject;
	class Transform;
	struct Vector3;
	/// <summary>
			/// ウィンドウからのメッセージを受信してImGuiで入力やイベントを処理するためのコールバック関数
			/// </summary>
			/// <param name="hwnd">ウィンドウハンドル</param>
			/// <param name="msg">メッセージ</param>
			/// <param name="wParam">パラメータ</param>
			/// <param name="lParam">パラメータ</param>
			/// <returns></returns>
	//IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	class ImGuizmoManipulator : public ImGuiShowable
	{
		friend class ImGuiRenderer;
	public:
		ImGuizmoManipulator();
		
		void SetCamera();
		void Initialize();
		void ShowImGui() override;
		void UpdateCamera(const char* _name);
		bool IsMouseInWindow(const char* _name);
		void GetMouseRay(Vector3* _near, Vector3* _far);
		void SelectTransform();

	private:
		void DrawTransformGuizmo();
		void Calculate();
		ImGuizmo::OPERATION operation_;
		ImGuizmo::MODE mode_;
		GameObject* pCamera_;
		Transform* pCameraTransform_;
		Transform* pTargetTransform_;
		float angleX_;
		float angleY_;
		CameraHandleInScene hCamera_;
		uintptr_t currId_;
	private:
		float worldMat_[16], viewMat_[16], projMat_[16];
		Matrix4x4 worldMatrix4x4, viewMatrix4x4_, projMatrix4x4_;
		DirectX::XMFLOAT4X4 float4x4_;

	};
	class ImGuiRenderer final : public ISystem
	{
	public:
		enum class WindowFlag
		{
			None,
			NoMoveWhenHovered // マウスカーソルがウィンドウ内にあるとき移動禁止
		};
	public:
	
		ImGuiRenderer();
		~ImGuiRenderer();
		void Initialize() override;
		void Update() override;
		void BeginFrame();
		void UpdateCamera(const char* _name);
		void BeginImGuizmoFrame();

		void Begin(const char* _str);
		void Begin(const char* _str,WindowFlag _flag);
		
		/// <summary>
		/// ImGuizmoウィンドウを描画するためにRTVをセット
		/// </summary>
		void SetImGuizmoRenderTargetView();

		void SetGameViewCamera();
		void Draw();
		
		void EndFrame();
		void SetDrawList();
		
		void RenderSceneView();
		bool IsHoveringWindow();
		/*bool IsMouseInGameView();*/
		void UpdateGameViewRect();
		void End();
		void Release();
		
		const D3D11_VIEWPORT& GetViewport() { return viewport_; }
	private:
		UINT winWidth_, winHeight_;
		bool isManipulatingGuizmo_;
		ImGuizmoManipulator* manipulator_;
		
		// Game Viewウィンドウの前フレーム情報
		ImVec2 gameViewPos_;
		ImVec2 gameViewSize_;
		bool gameViewRectValid_;

		//ImGuiIO io;
		ComPtr<ID3D11RenderTargetView> pRenderTargetView_;
		ComPtr<ID3D11ShaderResourceView> pSRV_;
		ComPtr<ID3D11Texture2D> pTexture_;
		ComPtr<ID3D11Texture2D> pDepthStencil_;
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_;
		D3D11_VIEWPORT viewport_;
	};
}