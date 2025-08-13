#include "MTImGui.h"
#include "Game.h"
#include "WindowContextResourceManager.h"
#include "WindowContextUtil.h"
#include "DirectX11Draw.h"
#include "DirectX11Manager.h"
#include "../ImGui\imgui.h"
#include "../ImGui\imgui_internal.h"
#include <cmath>
#include <algorithm>
#include "Debug.h"
namespace
{
	float titleBarHeight;
	const unsigned long long NULLID = 0;
	const char* GAME_VIEW_NAME = "Game View";
}

mtgb::Vector3 mtgb::QuatToEuler(DirectX::XMVECTORF32 _q)
{
	//分母、分子
	float denom, num;
	float roll, pitch, yaw;
	float x = _q[0], y = _q[1], z = _q[2], w = _q[3];

	//ピッチ(x軸)
	
	//90～^90の範囲
	float sinX = 2.0f * (w * y - z * x);
	pitch = std::asinf(std::clamp(sinX,-1.0f,1.0f));

	//ヨー(y軸)
	num = 2.0f * (w * z + x * y);
	denom = 1 - 2.0f * (y * y + z * z);
	yaw = std::atan2f(num, denom);

	//ロール(z軸)
	num = 2.0f * (w * x + y * z);
	denom = 1 - 2.0f * (x * x + y * y);
	roll = std::atan2(num, denom);

	float pitch_deg = DirectX::XMConvertToDegrees(pitch);
	float yaw_deg = DirectX::XMConvertToDegrees(yaw);
	float roll_deg = DirectX::XMConvertToDegrees(roll);

	return { pitch_deg,yaw_deg,roll_deg};
}

mtgb::MTImGui::MTImGui()
	:pRenderTargetView_{nullptr}
	,pSRV_{nullptr}
	,pTexture_{nullptr}
	,pDepthStencil_{nullptr}
	,pDepthStencilView_{nullptr}
	,winWidth_{640}
	,winHeight_{480}
	,gameViewRectValid_{false}
{
}
mtgb::MTImGui::~MTImGui()
{
}

void mtgb::MTImGui::Initialize()
{
	IMGUI_CHECKVERSION();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();(void)io;

	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	//io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

	ImGui::SetNextWindowSize(ImVec2(winWidth_, winHeight_), ImGuiCond_Once);
	ImGui_ImplWin32_Init(WinCtxRes::GetHWND(WindowContext::First));
	const auto& ctx = ImGui::GetCurrentContext();
	ComPtr<ID3D11Device> device = mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;
	ImGui_ImplDX11_Init(device.Get(), context.Get());

	currId_ = NULLID;
	manipulator_ = new ImGuizmoManipulator();

	//テクスチャ作成

	D3D11_TEXTURE2D_DESC desc
	{
		.Width = winWidth_,
		.Height = winHeight_,
		.MipLevels = 1,
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc
		{
			.Count = 1,
		},
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
	};
	ID3D11Texture2D* pRawTexture = nullptr;
	device->CreateTexture2D(&desc, nullptr, &pRawTexture);
	pTexture_.Attach(pRawTexture);

	//SRV作成
	ID3D11ShaderResourceView* pRawSRV = nullptr;
	device->CreateShaderResourceView(pTexture_.Get(), nullptr, &pRawSRV);
	pSRV_.Attach(pRawSRV);

	//RenderTargetView作成
	ID3D11RenderTargetView* pRawRenderTargetView = nullptr;
	device->CreateRenderTargetView(pTexture_.Get(), nullptr, &pRawRenderTargetView);
	pRenderTargetView_.Attach(pRawRenderTargetView);


	// 深度ステンシルと深度ステンシルビューを作成
	ID3D11Texture2D* pRawDepthStencil = nullptr;
	ID3D11DepthStencilView* pRawDepthStencilView = nullptr;
	Game::System<DirectX11Manager>().CreateDepthStencilAndDepthStencilView(Vector2Int(winWidth_, winHeight_), &pRawDepthStencil, &pRawDepthStencilView);
	pDepthStencil_.Attach(pRawDepthStencil);
	pDepthStencilView_.Attach(pRawDepthStencilView);
}

void mtgb::MTImGui::Update()
{
	//EndFrame();
	//BeginFrame();
	//ImGui::ShowDemoWindow();
}

void mtgb::MTImGui::BeginFrame()
{
	
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
}
void mtgb::MTImGui::BeginImGuizmoFrame()
{
	ImGuizmo::BeginFrame();
	
}
void mtgb::MTImGui::Begin(std::string str)
{
	ImGui::Begin(str.c_str());
}
void mtgb::MTImGui::Begin(std::string str, ImGuiWindowFlags flag)
{
	ImGui::Begin(str.c_str(), 0, flag);
}
void mtgb::MTImGui::SetImGuizmoRenderTargetView()
{
	Game::System<DirectX11Manager>().ChangeRenderTargets(pRenderTargetView_,pDepthStencilView_);
}
void mtgb::MTImGui::Draw()
{

}

void mtgb::MTImGui::EndFrame()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void mtgb::MTImGui::SetDrawList()
{
	ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
	
}

void mtgb::MTImGui::BeginGameView()
{
	ImGuiWindowFlags flags = 0;
	if (IsMouseInGameView())
	{
		flags |= ImGuiWindowFlags_NoMove;
	}
	ImGui::Begin(GAME_VIEW_NAME, 0, flags);
}

void mtgb::MTImGui::RenderGameView()
{
	ImGui::Image((void*)pSRV_.Get(), ImVec2(winWidth_, winHeight_));
}

bool mtgb::MTImGui::IsMouseInGameView()
{
	//	前フレームの矩形情報を使用
	/*if (gameViewRectValid_)
	{
		ImVec2 mousePos = ImGui::GetIO().MousePos;
		return mousePos.x >= gameViewPos_.x && mousePos.x <= gameViewPos_.x + gameViewSize_.x &&
			   mousePos.y >= gameViewPos_.y && mousePos.y <= gameViewPos_.y + gameViewSize_.y;
	}*/
	
	//	GameViewウィンドウを検索
	ImGuiWindow* window = ImGui::FindWindowByName(GAME_VIEW_NAME);
	if (window && window->WasActive)
	{
		ImVec2 mousePos = ImGui::GetIO().MousePos;
		ImRect rect = window->InnerRect;
		return rect.Contains(mousePos);
	}
	
	return false;
}

void mtgb::MTImGui::UpdateGameViewRect()
{
	gameViewPos_ = ImGui::GetWindowPos();
	gameViewSize_ = ImGui::GetWindowSize();
	gameViewRectValid_ = true;
}



void mtgb::MTImGui::End()
{
	ImGui::End();
}
void mtgb::MTImGui::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

bool mtgb::MTImGui::DrawTransformGuizmo(uintptr_t _ptrId, float* _worldMat, const float* _viewMat, const float* _projMat, DirectX::XMFLOAT3* _position, DirectX::XMVECTORF32* _rotation, DirectX::XMFLOAT3* _scale)
{
	ImGui::PushID(&_ptrId);

	ImVec2 pos = ImGui::GetWindowPos();
	float windowWidth = (float)ImGui::GetWindowWidth();
	float windowHeight = (float)ImGui::GetWindowHeight();
	
	
	//ギズモ表示
	ImGuizmo::SetRect(pos.x, pos.y , ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
	
	
	bool ret = false;
	if (ImGuizmo::IsUsing())
	{
		if (currId_ == NULLID)
		{
			currId_ = _ptrId;
		}
		if (currId_ == _ptrId)
		{
			if (ImGuizmo::Manipulate(_viewMat, _projMat, manipulator_->operation_, manipulator_->mode_, _worldMat))
			{
				//編集されたworldMatからposition,rotation,scaleに分解
				DirectX::XMMATRIX mat = DirectX::XMMATRIX(
					_worldMat[0], _worldMat[1], _worldMat[2], _worldMat[3],
					_worldMat[4], _worldMat[5], _worldMat[6], _worldMat[7],
					_worldMat[8], _worldMat[9], _worldMat[10], _worldMat[11],
					_worldMat[12], _worldMat[13], _worldMat[14], _worldMat[15]
				);

				DirectX::XMVECTOR scale, trans;
				bool result = DirectX::XMMatrixDecompose(&scale, &_rotation->v, &trans, mat);
				massert(result
					&& "XMMatrixDecomposeに失敗 @MTImGui::DrawTransformGuizmo");

				DirectX::XMStoreFloat3(_position, trans);
				DirectX::XMStoreFloat3(_scale, scale);

				ret = true;

			}
		}
	}
	else
	{
		currId_ = NULLID;
	}
	
	ImGui::PopID();
	return ret;
}

mtgb::ImGuizmoManipulator::ImGuizmoManipulator()
	:ImGuiShowable("Manipulater",Show::GameView)
	, operation_{ ImGuizmo::TRANSLATE }
	, mode_{ImGuizmo::LOCAL}
{}

void mtgb::ImGuizmoManipulator::ShowImGui()
{
	//ImGuizmoの操作モードを指定

	if (ImGui::RadioButton("Translate", operation_ == ImGuizmo::TRANSLATE))
	{
		operation_ = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", operation_ == ImGuizmo::ROTATE))
	{
		operation_ = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", operation_ == ImGuizmo::SCALE))
	{
		operation_ = ImGuizmo::SCALE;
	}

	if (ImGui::RadioButton("Local", mode_ == ImGuizmo::LOCAL))
	{
		mode_ = ImGuizmo::LOCAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("World", mode_ == ImGuizmo::WORLD))
	{
		mode_ = ImGuizmo::WORLD;
	}
}
