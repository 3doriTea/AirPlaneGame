#include "MTImGui.h"
#include "Game.h"
#include "MainWindow.h"
#include "DirectX11Draw.h"

mtgb::MTImGui::~MTImGui()
{
	Release();
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

	ImGui_ImplWin32_EnableDpiAwareness();
	float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));
	// Setup scaling
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
	style.FontScaleDpi = main_scale;        // Set initial font scale. (using 

	io.ConfigDpiScaleFonts = true;
	io.ConfigDpiScaleViewports = true;
	
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	ImGui::SetCurrentContext(ImGui::GetCurrentContext());

	MainWindow& mainWindow = Game::System<MainWindow>();
	
	ImGui_ImplWin32_Init(mainWindow.GetHWND());
	ImGui_ImplDX11_Init(mtgb::DirectX11Draw::pDevice_, mtgb::DirectX11Draw::pContext_);
	BeginFrame();
}

void mtgb::MTImGui::Update()
{
	EndFrame();
	BeginFrame();
	//ImGui::ShowDemoWindow();
}

void mtgb::MTImGui::BeginFrame()
{
	ImGui::SetCurrentContext(ImGui::GetCurrentContext());
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("a");
	/*if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::Text("This is some useful text.");
	}
	else
	{
		ImGui::Text("This is.");
	}*/
}

void mtgb::MTImGui::EndFrame()
{
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
	//BeginFrame()
	
	//EndDraw
	//BeginDraw
	//EndFrame
	//BeginFrame()
}

void mtgb::MTImGui::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
