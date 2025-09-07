#include "DoubleWindow.h"
#include "WindowContext.h"
#include "WindowManager.h"
#include "string"
#include "Game.h"
#include "Screen.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "WindowContextResourceManager.h"
#include "WindowContextUtil.h"
#include "WindowResource.h"
mtgb::DoubleWindow::DoubleWindow()
{
	context1_ = WindowContext::First;
	context2_ = WindowContext::Second;
}

mtgb::DoubleWindow::~DoubleWindow()
{
}

void mtgb::DoubleWindow::Initialize()
{
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	
	WindowConfig config1 =
	{
		.title = "Player1",
		.className = "Player1WindowClass",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = 0,
		.y = 0
	};

	WindowConfig config2 =
	{
		.title = "Player2",
		.className = "Player2WindowClass",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = config1.x + config1.width,
		.y = 0
	};

	//mtgb::WindowManager::CreateWindowContext(config1, &context1_);
	//mtgb::WindowManager::CreateWindowContext(config2, &context2_);

	//リソースの初期化もここで行う
	Game::System<WindowManager>().SetWindowConfig(WindowContext::First,config1);
	Game::System<WindowManager>().SetWindowConfig(WindowContext::Second,config2);
	

	Game::System<WindowContextResourceManager>().CreateResource(WindowContext::First);
	Game::System<WindowContextResourceManager>().CreateResource(WindowContext::Second);

	HWND hWnd1 = WinCtxRes::GetHWND(WindowContext::First);
	HWND hWnd2 = WinCtxRes::GetHWND(WindowContext::Second);

	ShowWindow(hWnd1, SW_SHOW);
	ShowWindow(hWnd2, SW_SHOW);

	// ウィンドウの表示、初期化完了をマーク
	Game::System<WindowContextResourceManager>().Get<WindowResource>(WindowContext::First).MarkInitialized();
	Game::System<WindowContextResourceManager>().Get<WindowResource>(WindowContext::Second).MarkInitialized();

	Game::System<WindowContextResourceManager>().ChangeActiveResource(WindowContext::First);

	SetDoubleWindowPos();

	SetForegroundWindow(hWnd1);
}

void mtgb::DoubleWindow::Update()
{
	
}

void mtgb::DoubleWindow::SetDoubleWindowPos()
{
	// モニターサイズ取得
	RECT monitorRect;
	GetWindowRect(GetDesktopWindow(), &monitorRect);
	int monitorWidth = monitorRect.right - monitorRect.left;

	HWND hWnd1 = WinCtxRes::GetHWND(WindowContext::First);
	HWND hWnd2 = WinCtxRes::GetHWND(WindowContext::Second);

	// ウィンドウサイズ取得
	RECT win1Rect, win2Rect;
	GetWindowRect(hWnd1, &win1Rect);
	GetWindowRect(hWnd2, &win2Rect);
	int win1Width = win1Rect.right - win1Rect.left;
	int win2Width = win2Rect.right - win2Rect.left;

	int win1X = 0;
	int win2X = 0;
	//二つ目のウィンドウも収まるなら一つ目の横に並べて配置、見切れるならモニターの右端に合わせる
	if (win1Width + win2Width <= monitorWidth) {
		win2X = win1X + win1Width; // 並べる
	}
	else {
		WindowConfig config = Game::System<WindowManager>().GetWindowConfig(WindowContext::Second);
		win2X = monitorRect.right - config.width; // 右端に合わせる
	}

	// 位置調整
	SetWindowPos(hWnd1, nullptr, win1X, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(hWnd2, nullptr, win2X, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);


}





