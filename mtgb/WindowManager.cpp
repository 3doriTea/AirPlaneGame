#include "WindowManager.h"
#include "WindowContextResourceManager.h"
#include "WindowContext.h"
#include <Windows.h>
#include "MTAssert.h"
#include "WindowResource.h"
#include "Game.h"
#include "ISystem.h"
namespace
{
	
}

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
	WNDCLASSEXW windowClass{};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.lpszClassName = config.className;
	windowClass.lpfnWndProc = mtgb::WindowResource::WndProc; // 仮のプロシージャ
	windowClass.style = CS_VREDRAW | CS_HREDRAW;
	windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	windowClass.hIconSm = LoadIcon(nullptr, IDI_WINLOGO);
	windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	windowClass.lpszMenuName = nullptr;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	massert(RegisterClassExW(&windowClass) != 0
		&& "RegisterClassExWに失敗 @WindowManager::CreateWindowContext");

	RECT windowRect{ 0, 0, config.width, config.height };
	massert(
		AdjustWindowRectEx(
			&windowRect,
			WS_OVERLAPPEDWINDOW,
			FALSE,
			WS_EX_OVERLAPPEDWINDOW) != FALSE
		&& "AdjustWindowRectExに失敗 @WindowManager::CreateWindowContext");

	WindowResource& windowResource = WindowManager::GetWindowResource(context);

	HWND hWnd = CreateWindowExW(
		0,
		config.className,
		config.title,
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

	ShowWindow(hWnd, SW_SHOW);
	
	return hWnd;
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

void mtgb::WindowManager::RegisterWindowConfig(WindowContext windowContext, const WindowConfig& config)
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
