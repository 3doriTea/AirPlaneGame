#include "ImGuiRenderer.h"
#include "Game.h"
#include "ISystem.h"
#include "ColliderCP.h"
#include "CameraSystem.h"
#include "DirectX11Draw.h"
#include "DirectX11Manager.h"
#include "GameObject.h"
#include "WindowContextUtil.h"
#include "GameTime.h"
#include "InputData.h"
#include "Vector3.h"
#include "../ImGui/imgui_impl_win32.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../ImGui\imgui_internal.h"
#include <cmath>
#include "QuatToEuler.h"
#include "InputResource.h"
#include "Screen.h"
namespace
{
	float titleBarHeight;
	const unsigned long long NULLID = 0;
	const char* WINDOWNAME_GAME_VIEW = "Game View";
}





mtgb::ImGuiRenderer::ImGuiRenderer()
	:pRenderTargetView_{nullptr}
	,pSRV_{nullptr}
	,pSRVTexture_{nullptr}
	,pTexture_{nullptr}
	,pDepthStencil_{nullptr}
	,pDepthStencilView_{nullptr}
	,gameViewRectValid_{false}
	
{
	
}
mtgb::ImGuiRenderer::~ImGuiRenderer()
{
}

void mtgb::ImGuiRenderer::Initialize()
{
	Vector2Int size = Game::System<Screen>().GetSize();
	winWidth_ = static_cast<UINT>(size.x);
	winHeight_ = static_cast<UINT>(size.y);

	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();(void)io;

	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	
	ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != nullptr);

	//ImGui_ImplWin32_EnableDpiAwareness();
	//float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
	//// Setup scaling
	//ImGuiStyle& style = ImGui::GetStyle();
	//style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	//style.FontScaleDpi = main_scale;        // Set initial font scale. (using 

	//io.ConfigDpiScaleFonts = true;
	//io.ConfigDpiScaleViewports = true;
	
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	ImGui_ImplWin32_Init(WinCtxRes::GetHWND(WindowContext::First));
	const auto& ctx = ImGui::GetCurrentContext();
	ComPtr<ID3D11Device> device = mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;
	ImGui_ImplDX11_Init(device.Get(), context.Get());

	manipulator_ = new ImGuizmoManipulator();

	CreateD3DResources();
}

void mtgb::ImGuiRenderer::Update()
{

}
void mtgb::ImGuiRenderer::UpdateCamera(const char* _name)
{
	manipulator_->UpdateCamera(_name);
}
void mtgb::ImGuiRenderer::BeginFrame()
{
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
}
void mtgb::ImGuiRenderer::BeginImGuizmoFrame()
{
	ImGuizmo::BeginFrame();
}



void mtgb::ImGuiRenderer::Begin(const char* _str, bool* _isOpen, WindowFlag _flag)
{
	ImGuiWindowFlags flags = 0;
	if (_flag == WindowFlag::NoMoveWhenHovered)
	{
		if (manipulator_->IsMouseInWindow(_str))
		{
			flags |= ImGuiWindowFlags_NoMove;
		}
	}
	ImGui::Begin(_str, _isOpen, flags);
}


void mtgb::ImGuiRenderer::SetImGuizmoRenderTargetView()
{
	//ID3D11ShaderResourceView* nullSRV[16]{};
	//// 全スロットのSRVをクリア
	//DirectX11Draw::pContext_->PSSetShaderResources(0, 16, nullSRV);

	//DirectX11Draw::pContext_->PSSetShaderResources(0, 0, pSRV_.GetAddressOf());
	//Game::System<DirectX11Manager>().ChangeViewport(viewport_);
	Game::System<DirectX11Manager>().ChangeRenderTargets(pRenderTargetView_,pDepthStencilView_);
}
void mtgb::ImGuiRenderer::SetGameViewCamera()
{
	Game::System<CameraSystem>().SetDrawCamera(manipulator_->hCamera_);
}
void mtgb::ImGuiRenderer::Draw()
{

}

void mtgb::ImGuiRenderer::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void mtgb::ImGuiRenderer::SetDrawList()
{
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
}

void mtgb::ImGuiRenderer::RenderSceneView()
{
	
	ImGui::Image((void*)pSRV_.Get(), ImVec2(static_cast<float>(winWidth_), static_cast<float>(winHeight_)));
}



bool mtgb::ImGuizmoManipulator::IsMouseInWindow(const char* _name)
{
	//	前フレームの矩形情報を使用
	/*if (gameViewRectValid_)
	{
		ImVec2 mousePos = ImGui::GetIO().MousePos;
		return mousePos.x >= gameViewPos_.x && mousePos.x <= gameViewPos_.x + gameViewSize_.x &&
			   mousePos.y >= gameViewPos_.y && mousePos.y <= gameViewPos_.y + gameViewSize_.y;
	}*/

	//	ウィンドウを検索
	ImGuiWindow* window = ImGui::FindWindowByName(_name);
	if (window && window->WasActive)
	{
		ImVec2 mousePos = ImGui::GetIO().MousePos;
		ImRect rect = window->InnerRect;
		return rect.Contains(mousePos);
	}
	return false;
}

void mtgb::ImGuizmoManipulator::GetMouseRay(Vector3* _near, Vector3* _far)
{

	Game::System<CameraSystem>().GetViewMatrix(&viewMatrix4x4_);
	Game::System<CameraSystem>().GetProjMatrix(&projMatrix4x4_);

	ImVec2 mousePos = ImGui::GetMousePos();
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 gameViewSize_ = ImGui::GetWindowSize();
	
	float tabBarHeight = ImGui::GetCurrentWindow()->TitleBarHeight;
	
	ImVec2 localPos = ImVec2(mousePos.x - windowPos.x, mousePos.y - windowPos.y - tabBarHeight);

	Vector3 nearVec = {localPos.x,localPos.y,0.0f };
	const D3D11_VIEWPORT& viewport = Game::System<ImGuiRenderer>().GetViewport();
	*_near = DirectX::XMVector3Unproject(
		nearVec,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		projMatrix4x4_,
		viewMatrix4x4_,
		DirectX::XMMatrixIdentity());

	Vector3 farVec = { static_cast<float>(localPos.x),static_cast<float>(localPos.y),1.0f };
	*_far = DirectX::XMVector3Unproject(
		farVec,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		projMatrix4x4_,
		viewMatrix4x4_,
		DirectX::XMMatrixIdentity());

}

void mtgb::ImGuizmoManipulator::SelectTransform()
{

	Vector3 origin, end, vec;
	GetMouseRay(&origin, &end);
	vec = end - origin;
	
	// vec.Normalize()の結果を別変数に保存して、元の長さを保持
	Vector3 direction = vec.Normalize();  // これで正規化されたベクトルが返される

	const CameraSystem& camera = Game::System<CameraSystem>();
	float distance = camera.GetFar() - camera.GetNear();          // 元の長さを計算

	EntityId entityId = Game::System<ColliderCP>().RayCastHitAll(origin, direction, distance);
	if (entityId != INVALD_ENTITY)
	{
		// EntityがTransformコンポーネントを持っていない可能性があるのでTryGet
		Game::System<TransformCP>().TryGet(pTargetTransform_, entityId);
	}
	else
	{
		pTargetTransform_ = nullptr;
	}
}




void mtgb::ImGuiRenderer::UpdateGameViewRect()
{
	gameViewPos_ = ImGui::GetWindowPos();
	gameViewSize_ = ImGui::GetWindowSize();
	gameViewRectValid_ = true;
}

void mtgb::ImGuiRenderer::End()
{
	ImGui::End();
}
void mtgb::ImGuiRenderer::Release()
{
	SAFE_DELETE(manipulator_);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void mtgb::ImGuiRenderer::ResetComPtrs()
{
	// ImGuiが保持しているリソース解放
	ImGui_ImplDX11_InvalidateDeviceObjects();

	pSRV_.Reset();
	pRenderTargetView_.Reset();
	pDepthStencilView_.Reset();
	pDepthStencil_.Reset();
	pTexture_.Reset();

}

void mtgb::ImGuiRenderer::OnResize(UINT width, UINT height)
{
	CreateD3DResources();
    // ウィンドウサイズを更新
    /*winWidth_ = width;
    winHeight_ = height;*/
    
    // GameView の矩形情報をリセット
    gameViewRectValid_ = false;
    
    // ImGuiのディスプレイサイズを更新
    /*ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(width), static_cast<float>(height));*/
    
    // ImGuiのリソースを再作成する必要がある場合
    // ImGui_ImplDX11_InvalidateDeviceObjects();
    // ImGui_ImplDX11_CreateDeviceObjects();
}

void mtgb::ImGuiRenderer::CreateD3DResources()
{
	ComPtr<ID3D11Device> device = mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;

	Game::System<DirectX11Manager>().CreateViewport({ static_cast<int>(winWidth_) ,static_cast<int>(winHeight_) }, viewport_);

	// RTV用テクスチャ作成
	D3D11_TEXTURE2D_DESC desc
	{
		.Width = winWidth_,
		.Height = winHeight_,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc = { .Count = 1 },
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
	};
	
	HRESULT hResult = device->CreateTexture2D(&desc, nullptr, pTexture_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateTexture2Dに失敗 @ImGuiRenderer::CreateD3DResources");

	hResult = device->CreateRenderTargetView(pTexture_.Get(), nullptr, pRenderTargetView_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateRenderTargetViewに失敗 @ImGuiRenderer::CreateD3DResources");

	// SRV用テクスチャ作成
	//desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//hResult = device->CreateTexture2D(&desc, nullptr, pTexture_.GetAddressOf());
	//massert(SUCCEEDED(hResult) && "CreateTexture2Dに失敗 @ImGuiRenderer::CreateD3DResources");

	hResult = device->CreateShaderResourceView(pTexture_.Get(), nullptr, pSRV_.ReleaseAndGetAddressOf());
	massert(SUCCEEDED(hResult) && "CreateShaderResourceViewに失敗 @ImGuiRenderer::CreateD3DResources");

	// 深度ステンシルと深度ステンシルビューを作成
	Game::System<DirectX11Manager>().CreateDepthStencilAndDepthStencilView(
		Vector2Int(static_cast<int>(winWidth_), static_cast<int>(winHeight_)),
		pTexture_.GetAddressOf(), pDepthStencilView_.ReleaseAndGetAddressOf());

	//ImGui_ImplDX11_CreateDeviceObjects();
}





