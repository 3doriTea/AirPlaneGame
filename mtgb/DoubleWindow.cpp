#include "DoubleWindow.h"
#include "WindowContext.h"
#include "WindowManager.h"
#include "string"
#include "Game.h"
#include "Screen.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "WindowContextResourceManager.h"
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
		.x = CW_USEDEFAULT,
		.y = CW_USEDEFAULT
	};

	WindowConfig config2 =
	{
		.title = "Player2",
		.className = "Player2WindowClass",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = CW_USEDEFAULT,
		.y = CW_USEDEFAULT
	};

	//mtgb::WindowManager::CreateWindowContext(config1, &context1_);
	//mtgb::WindowManager::CreateWindowContext(config2, &context2_);

	//一旦DirectXリソースの初期化もここで行う
	Game::System<WindowManager>().RegisterWindowConfig(context1_,config1);
	Game::System<WindowManager>().RegisterWindowConfig(context2_,config2);
	

	Game::System<WindowContextResourceManager>().CreateResource(context1_);
	Game::System<WindowContextResourceManager>().CreateResource(context2_);

	ChangeFirstWindow();
}

void mtgb::DoubleWindow::Update()
{
	//一旦context1を従来のウィンドウにして、context2は何も表示しない
	/*DirectX11Draw::End();

	Game::System<DirectX11Manager>().ChangeRenderContext(*context2_);
	DirectX11Draw::Begin();
	DirectX11Draw::End();

	Game::System<DirectX11Manager>().ChangeRenderContext(*context1_);
	DirectX11Draw::Begin();*/
}

const HWND mtgb::DoubleWindow::GetFirstWindowHandle()
{
	return Game::System<WindowContextResourceManager>().GetHWND(context1_);
}

const HWND mtgb::DoubleWindow::GetSecondWindowHandle()
{
 	return Game::System<WindowContextResourceManager>().GetHWND(context2_);
}

void mtgb::DoubleWindow::ChangeFirstWindow()
{
	//Game::System<DirectX11Manager>().ChangeRenderContext(*context1_);
	Game::System<WindowContextResourceManager>().ChangeResource(context1_);
}

void mtgb::DoubleWindow::ChangeSecondWindow()
{
	//Game::System<DirectX11Manager>().ChangeRenderContext(*context2_);
	Game::System<WindowContextResourceManager>().ChangeResource(context2_);
}
