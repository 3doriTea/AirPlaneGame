#include "WindowManager.h"
#include "WindowContextResourceManager.h"
#include "WindowContext.h"
//#include <Windows.h>
//#include "WindowRenderContext.h"
#include "IncludingWindows.h"
#include "MTAssert.h"
#include "WindowResource.h"
#include "Game.h"
#include "ISystem.h"
#include "MTStringUtility.h"
#include "ImGuiRenderer.h"

MSG* mtgb::WindowManager::pPeekedMessage_{ nullptr };
std::map<mtgb::WindowContext, mtgb::WindowConfig> mtgb::WindowManager::windowConfigMap_;

mtgb::WindowManager::WindowManager() 
{
	pPeekedMessage_ = new MSG{};
}

mtgb::WindowManager::~WindowManager()
{
	delete pPeekedMessage_;
}

HWND mtgb::WindowManager::CreateWindowContext(WindowContext context)
{
	WindowConfig config = WindowManager::GetWindowConfig(context);
	// ウィンドウ作成処理
	WNDCLASSEX windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpszClassName = config.className.c_str();
	windowClass.lpfnWndProc = mtgb::WindowResource::WndProc; // 仮のプロシージャ
	windowClass.style = CS_VREDRAW | CS_HREDRAW;
	windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszMenuName = nullptr;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	massert(RegisterClassEx(&windowClass) != 0
		&& "RegisterClassExに失敗 @WindowManager::CreateWindowContext");

	RECT windowRect{ 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE
		&& "AdjustWindowRectExに失敗 @WindowManager::CreateWindowContext");

	WindowResource& windowResource = WindowManager::GetWindowResource(context);

	HWND hWnd = CreateWindowEx(
		0,
		config.className.c_str(),
		config.title.c_str(),
		WS_OVERLAPPEDWINDOW,
		config.x,
		config.y,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		GetModuleHandle(NULL),
		reinterpret_cast<LPVOID>(&windowResource)
	);

	massert(hWnd != NULL
		&& "ウィンドウの作成に失敗");
	massert(SetWindowText(hWnd, config.title.c_str())
		&& "SetWindowTextに失敗");

	
	return hWnd;
}

mtgb::Vector2Int mtgb::WindowManager::GetWindowSize(WindowContext context)
{
	if (context == WindowContext::Both)
	{
		return mtgb::Vector2Int{ windowConfigMap_[WindowContext::First].width,windowConfigMap_[WindowContext::First].height };
	}
	return mtgb::Vector2Int{ windowConfigMap_[context].width,windowConfigMap_[context].height };
}

void mtgb::WindowManager::Initialize()
{
}

void mtgb::WindowManager::Update()
{
	if (PeekMessage(
		pPeekedMessage_,
		nullptr,
		0u,
		0u,
		PM_REMOVE))
	{
		TranslateMessage(pPeekedMessage_);
		DispatchMessage(pPeekedMessage_);
		return;
	}
}

void mtgb::WindowManager::Release()
{
}

void mtgb::WindowManager::SetWindowConfig(WindowContext windowContext, const WindowConfig& config)
{
	windowConfigMap_[windowContext] = config;
}

mtgb::WindowConfig mtgb::WindowManager::GetWindowConfig(WindowContext windowContext)
{
	auto itr = windowConfigMap_.find(windowContext);
	massert(itr != windowConfigMap_.end() && "指定されたWindowContextのConfigが見つかりません");
	return itr->second;
}

mtgb::WindowResource& mtgb::WindowManager::GetWindowResource(WindowContext windowContext)
{
	return Game::System<WindowContextResourceManager>().Get<WindowResource>(windowContext);
}

void mtgb::WindowManager::ResizeWindow(WindowContext _windowContext, UINT _width, UINT _height)
{
	// パイプラインにバインドした設定、スワップチェーンのバックバッファを参照するリソースのリセット
	Game::System<DirectX11Manager>().ClearState();
	if (_windowContext == WindowContext::First)
	{
		Game::System<ImGuiRenderer>().ResetComPtrs();
	}
	// リソースを更新
	Game::System<WindowContextResourceManager>().OnResizeAll(_windowContext, _width, _height);

	// デフォルトの設定をセット
	Game::System<DirectX11Manager>().SetDefaultStates();

	// ImGuiも更新
	if (_windowContext == WindowContext::First)
	{
		Game::System<ImGuiRenderer>().OnResize(_width,_height);
	}
}
