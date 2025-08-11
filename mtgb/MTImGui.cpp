#include "MTImGui.h"
#include "Game.h"
#include "WindowContextResourceManager.h"
#include "WindowContextUtil.h"
#include "DirectX11Draw.h"
#include "DirectX11Manager.h"
#include "../ImGui\ImGuizmo.h"
#include "../ImGui\imgui.h"
mtgb::MTImGui::MTImGui()
	:pRenderTargetView_{nullptr}
	,pSRV_{nullptr}
	,pTexture_{nullptr}
	,pDepthStencil_{nullptr}
	,pDepthStencilView_{nullptr}
	,winWidth_{640}
	,winHeight_{480}
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

	ImGui::SetNextWindowSize(ImVec2(winWidth_, winHeight_), ImGuiCond_Once);
	ImGui_ImplWin32_Init(WinCtxRes::GetHWND(WindowContext::First));
	const auto& ctx = ImGui::GetCurrentContext();
	ComPtr<ID3D11Device> device = mtgb::DirectX11Draw::pDevice_;
	ComPtr<ID3D11DeviceContext> context = mtgb::DirectX11Draw::pContext_;
	ImGui_ImplDX11_Init(device.Get(), context.Get());

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
	mtgb::DirectX11Draw::SetIsWriteToDepthBuffer(true);
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
	ImGuizmo::SetDrawlist(ImGui::GetForegroundDrawList());

}

void mtgb::MTImGui::RenderGameView()
{
	ImGui::Image((void*)pSRV_.Get(), ImVec2(winWidth_, winHeight_));
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

bool mtgb::MTImGui::DrawTransformGuizmo(float* worldMat, const float* viewMat, const float* projMat, DirectX::XMFLOAT3* position, DirectX::XMFLOAT3* rotation, DirectX::XMFLOAT3* scale)
{
	//ImGuizmoの操作モードを指定
	static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	static ImGuizmo::MODE mode = ImGuizmo::LOCAL;

	if (ImGui::RadioButton("Translate", operation == ImGuizmo::TRANSLATE))
	{
		operation = ImGuizmo::TRANSLATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", operation == ImGuizmo::ROTATE))
	{
		operation = ImGuizmo::ROTATE;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", operation == ImGuizmo::SCALE))
	{
		operation = ImGuizmo::SCALE;
	}

	if (ImGui::RadioButton("Local", mode == ImGuizmo::LOCAL))
	{
		mode = ImGuizmo::LOCAL;
	}
	ImGui::SameLine();
	if (ImGui::RadioButton("World", mode == ImGuizmo::WORLD))
	{
		mode = ImGuizmo::WORLD;
	}

	//ImVec2 pos = ImGui::GetWindowPos();
	//ImVec2 size = ImGui::GetWindowSize();
	//ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
	//ギズモ表示
	ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
	
	bool ret = false;
	if (ImGuizmo::Manipulate(viewMat, projMat, operation, mode, worldMat))
	{
		//編集されたworldMatからposition,rotation,scaleに分解
		float tempPos[3], tempRot[3], tempScale[3];
		ImGuizmo::DecomposeMatrixToComponents(worldMat, tempPos, tempRot, tempScale);

		//反映
		position->x = tempPos[0];
		position->y = tempPos[1];
		position->z = tempPos[2];

		rotation->x = tempRot[0];
		rotation->y = tempRot[1];
		rotation->z = tempRot[2];

		scale->x = tempScale[0];
		scale->y = tempScale[1];
		scale->z = tempScale[2];

		ret = true;
	}
	return ret;
}

