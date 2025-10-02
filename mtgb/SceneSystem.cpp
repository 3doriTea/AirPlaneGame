#include "SceneSystem.h"
#include "GameObject.h"
#include "WindowContextResourceManager.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "Input.h"
#include "ImGuiRenderer.h"
#include "WindowContext.h"
#include "WindowContextUtil.h"
#include "MTImGui.h"
#include "RenderSystem.h"
#include "InputData.h"
#include "WindowManager.h"
#include "CameraResource.h"
#include "InputResource.h"
#include "DXGIResource.h"
#include "Direct3DResource.h"
#include "WindowResource.h"

mtgb::SceneSystem::SceneSystem() :
	pNextScene_{ nullptr },
	onMoveListener_{}
{
}

mtgb::SceneSystem::~SceneSystem()
{
	SAFE_DELETE(GameScene::pInstance_);
}

void mtgb::SceneSystem::Initialize()
{
	OnMove([]() 
		{
			Game::System<Timer>().Clear();
		});
}

void mtgb::SceneSystem::Update()
{
	// 次のシーンが用意されているならシーンチェンジする
	if (pNextScene_)
	{
		ChangeScene();
	}

	if (GameScene::pInstance_ == nullptr)
	{
		return;  // シーンがないなら回帰
	}

	// 更新、描画前にコールバック実行
	ExecutePendingCallbacks();


	if (InputUtil::GetKeyDown(KeyCode::F11))
	{
		Game::System<WindowManager>().ChangeFullScreenState(WindowContext::First);
	}
	if (InputUtil::GetKeyDown(KeyCode::F10))
	{
		Game::System<WindowManager>().ChangeFullScreenState(WindowContext::Second);
	}
	if (InputUtil::GetKeyDown(KeyCode::F1))
	{
		MTImGui::Instance().ChangeAllWindowOpen();
	}
	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		Game::System<WinCtxResManager>().SwapResource<InputResource>();
	}
	if (InputUtil::GetKeyDown(KeyCode::F9))
	{
		WinCtxRes::SwapWindow();
		// CameraResourceは交換しない
		//Game::System<WinCtxResManager>().SwapResource<InputResource>();
	}
	if (InputUtil::GetKeyDown(KeyCode::F8))
	{
		WinCtxRes::SwapWindow();
		Game::System<WinCtxResManager>().SwapResource<InputResource>();
	}

	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		Game::System<Input>().EnumJoystick();

	}
	WinCtxRes::ChangeResource(WindowContext::First);
	Game::System<Input>().Update();
	Game::System<WindowContextResourceManager>().Update();

	WinCtxRes::ChangeResource(WindowContext::Second);
	Game::System<Input>().Update();
	Game::System<WindowContextResourceManager>().Update();

	// 現在のシーン
	GameScene& currentScene{ *GameScene::pInstance_ };

	// 更新処理
	currentScene.Update();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Update();
	}
	MTImGui::Instance().Update();

	// 描画処理
	Game::System<RenderSystem>().Render(currentScene);

	// 削除処理
	for (auto&& itr = currentScene.pGameObjects_.begin();
		itr != currentScene.pGameObjects_.end();)
	{
		if ((*itr)->IsToDestroy())
		{
			Game::RemoveEntityComponent((*itr)->GetEntityId());
			SAFE_DELETE(*itr);
			itr = currentScene.pGameObjects_.erase(itr);
		}
		else
		{
			itr++;
		}
	}
}

void mtgb::SceneSystem::RegisterPendingCallback(std::function<void()> _callback)
{
	pendingCallbacks_.push(_callback);
}

void mtgb::SceneSystem::ExecutePendingCallbacks()
{
	while (!pendingCallbacks_.empty())
	{
		pendingCallbacks_.front()();
		pendingCallbacks_.pop();
	}
}

void mtgb::SceneSystem::ChangeScene()
{
	// シーン遷移イベントを発動していく
	for (auto& onMove : onMoveListener_)
	{
		onMove();
	}

	// もし現在のシーンがあるなら終了処理
	if (GameScene::pInstance_)
	{
		GameScene::pInstance_->End();
	}

	// 解放してポインタ変更
	SAFE_DELETE(GameScene::pInstance_);
	GameScene::pInstance_ = pNextScene_;
	pNextScene_ = nullptr;

	// チェンジしたシーンの初期化処理
	GameScene::pInstance_->Initialize();
}
