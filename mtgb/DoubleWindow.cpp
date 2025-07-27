#include "DoubleWindow.h"
#include "WindowRenderContext.h"
#include "WindowManager.h"
#include "string"
#include "Game.h"
#include "Screen.h"
#include "DirectX11Manager.h"
mtgb::DoubleWindow::DoubleWindow()
{
	context1_ = nullptr;
	context2_ = nullptr;
}

mtgb::DoubleWindow::~DoubleWindow()
{
}

void mtgb::DoubleWindow::Initialize()
{
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

	WindowConfig config1 =
	{
		.title = L"Player1",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = CW_USEDEFAULT,
		.y = CW_USEDEFAULT
	};

	WindowConfig config2 =
	{
		.title = L"Player2",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = CW_USEDEFAULT,
		.y = CW_USEDEFAULT
	};

	mtgb::WindowManager::CreateWindowRenderContext(config1, &context1_);
	mtgb::WindowManager::CreateWindowRenderContext(config2, &context2_);

	//一旦DirectXリソースの初期化もここで行う
	Game::System<DirectX11Manager>().InitializeWindowContext(*context1_, false);
	Game::System<DirectX11Manager>().InitializeWindowContext(*context2_, false);
}

void mtgb::DoubleWindow::Update()
{
}
