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

	//mtgb::WindowManager::CreateWindowContext(config1, &context1_);
	//mtgb::WindowManager::CreateWindowContext(config2, &context2_);

	//��UDirectX���\�[�X�̏������������ōs��
	//Game::System<WindowContextResourceManager>().
	Game::System<WindowContextResourceManager>().CreateResource(context1_->hWnd_);
	Game::System<WindowContextResourceManager>().CreateResource(context2_->hWnd_);

	ChangeFirstWindow();
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

const HWND mtgb::DoubleWindow::GetFirstWindowHandle()
{
	return context1_->hWnd_;
}

const HWND mtgb::DoubleWindow::GetSecondWindowHandle()
{
	return context2_->hWnd_;
}

void mtgb::DoubleWindow::ChangeFirstWindow()
{
	//Game::System<DirectX11Manager>().ChangeRenderContext(*context1_);
	Game::System<WindowContextResourceManager>().ChangeResource(context1_->hWnd_);
}

void mtgb::DoubleWindow::ChangeSecondWindow()
{
	//Game::System<DirectX11Manager>().ChangeRenderContext(*context2_);
	Game::System<WindowContextResourceManager>().ChangeResource(context2_->hWnd_);
}
