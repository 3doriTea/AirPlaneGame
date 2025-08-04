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

	//リソースの初期化もここで行う
	Game::System<WindowManager>().RegisterWindowConfig(WindowContext::First,config1);
	Game::System<WindowManager>().RegisterWindowConfig(WindowContext::Second,config2);
	

	Game::System<WindowContextResourceManager>().CreateResource(WindowContext::First);
	Game::System<WindowContextResourceManager>().CreateResource(WindowContext::Second);

	Game::System<WindowContextResourceManager>().ChangeResource(WindowContext::First);
}

void mtgb::DoubleWindow::Update()
{
	
}


void mtgb::DoubleWindow::RegisterFirstWindowCamera()
{
	
}


