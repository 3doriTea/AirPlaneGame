#pragma once

#include <type_traits>
#include <assert.h>
#include <typeinfo>
#include <string>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "../ImGui/imgui.h"

#include "ISystem.h"
#include "IncludingWindows.h"
#include "ImGuiShowable.h"
#include "Handlers.h"
#include "ShowType.h"
#include "ImGuizmoManipulator.h"

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

		
		
		
		void Begin(const char* _str, bool* _isOpen = NULL ,WindowFlag _flag = WindowFlag::None);
		
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

		/// <summary>
		/// 
		/// </summary>
		void ResetComPtrs();
		/// <summary>
		/// ウィンドウリサイズ時の処理
		/// </summary>
		/// <param name="width">新しい幅</param>
		/// <param name="height">新しい高さ</param>
		void OnResize(UINT width, UINT height);
		
		const D3D11_VIEWPORT& GetViewport() { return viewport_; }
		ImGuizmoManipulator&  Manipulator() { return *manipulator_; };
	private:
		void CreateD3DResources();
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
		ComPtr<ID3D11Texture2D> pSRVTexture_;
		ComPtr<ID3D11Texture2D> pTexture_;
		ComPtr<ID3D11Texture2D> pDepthStencil_;
		ComPtr<ID3D11DepthStencilView> pDepthStencilView_;
		D3D11_VIEWPORT viewport_;
	};
}