#include "DoubleWindow.h"
#include "WindowContext.h"
#include "WindowManager.h"
#include "string"
#include "Game.h"
#include "Screen.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
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
		.title = L"tlayer1",
		.className = L"Player1WindowClass",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = CW_USEDEFAULT,
		.y = CW_USEDEFAULT
	};

	WindowConfig config2 =
	{
		.title = L"Jlayer2",
		.className = L"Player2WindowClass",
		.width = SCREEN_SIZE.x,
		.height = SCREEN_SIZE.y,
		.x = CW_USEDEFAULT,
		.y = CW_USEDEFAULT
	};

	mtgb::WindowManager::CreateWindowContext(config1, &context1_);
	mtgb::WindowManager::CreateWindowContext(config2, &context2_);

	//��UDirectX���\�[�X�̏������������ōs��
	Game::System<DirectX11Manager>().InitializeWindowContext(*context1_, false);
	Game::System<DirectX11Manager>().InitializeWindowContext(*context2_, false);

	Game::System<DirectX11Manager>().ChangeRenderContext(*context1_);
}

void mtgb::DoubleWindow::Update()
{
	//��Ucontext1���]���̃E�B���h�E�ɂ��āAcontext2�͉����\�����Ȃ�
	/*DirectX11Draw::End();

	Game::System<DirectX11Manager>().ChangeRenderContext(*context2_);
	DirectX11Draw::Begin();
	DirectX11Draw::End();

	Game::System<DirectX11Manager>().ChangeRenderContext(*context1_);
	DirectX11Draw::Begin();*/
}

const HWND mtgb::DoubleWindow::GetHWND()
{
	return context1_->hWnd_;
}

void mtgb::DoubleWindow::ChangeFirstWindow()
{
	Game::System<DirectX11Manager>().ChangeRenderContext(*context1_);
}

void mtgb::DoubleWindow::ChangeSecondWindow()
{
	Game::System<DirectX11Manager>().ChangeRenderContext(*context2_);
}
