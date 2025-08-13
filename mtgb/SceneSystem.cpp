#include "SceneSystem.h"
#include "GameObject.h"
#include "WindowContextResourceManager.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "Input.h"
#include "MTImGui.h"
#include "WindowContext.h"
#include "WindowContextUtil.h"
#include "ImGuiShowable.h"
#include "RenderSystem.h"
mtgb::SceneSystem::SceneSystem() :
	pNextScene_{ nullptr },
	onMoveListener_{}
{
}

mtgb::SceneSystem::~SceneSystem()
{
}

void mtgb::SceneSystem::Initialize()
{
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

	// 描画処理
	Game::System<RenderSystem>().Render(currentScene);
#if 0
	//一つ目のウィンドウ
	WinCtxRes::ChangeResource(WindowContext::First);

	MTImGui& imGui = Game::System<MTImGui>();

	//ImGui描画開始
	imGui.BeginFrame();
	imGui.BeginImGuizmoFrame();

	imGui.Begin("Window");

	//DirectX描画開始
	DirectX11Draw::Begin();

	//ImGuiウィジェット表示
	ImGuiShowSystem::Instance().ShowAll();

	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}
	imGui.End();
	DirectX11Draw::End();


	imGui.SetImGuizmoRenderTargetView();
	imGui.Begin("ImGuizmoWindow");
	DirectX11Draw::Begin();

	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}
	imGui.End();
	imGui.RenderGameView();

	imGui.EndFrame();

	DirectX11Draw::End();


	WinCtxRes::ChangeResource(WindowContext::Second);


	DirectX11Draw::Begin();
	
	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		if (gameObject->GetLayerFlag().Has(GameObjectLayer::B))
		{
			gameObject->Draw();
		}
	}
	
	DirectX11Draw::End();
#endif
	// 削除処理
	for (auto&& itr = currentScene.pGameObjects_.begin();
		itr != currentScene.pGameObjects_.end();)
	{
		if ((*itr)->IsToDestroy())
		{
			Game::RemoveEntityComponent((*itr)->GetEntityId());
			itr = currentScene.pGameObjects_.erase(itr);
		}
		else
		{
			itr++;
		}
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
