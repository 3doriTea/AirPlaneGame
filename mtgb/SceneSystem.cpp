#include "SceneSystem.h"
#include "GameObject.h"
#include "DoubleWindow.h"
#include "DirectX11Manager.h"
#include "DirectX11Draw.h"
#include "Input.h"

mtgb::SceneSystem::SceneSystem() :
	pNextScene_{ nullptr }
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

	Game::System<DoubleWindow>().ChangeFirstWindow();
	Game::System<Input>().Update();

	Game::System<DoubleWindow>().ChangeSecondWindow();
	Game::System<Input>().Update();

	// 現在のシーン
	GameScene& currentScene{ *GameScene::pInstance_ };

	// 更新処理
	currentScene.Update();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Update();
	}

	// 描画処理


	Game::System<DoubleWindow>().ChangeFirstWindow();
	DirectX11Draw::Begin();
	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}
	DirectX11Draw::End();

	Game::System<DoubleWindow>().ChangeSecondWindow();
	DirectX11Draw::Begin();
	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}
	DirectX11Draw::End();

	// 削除処理
	for (auto&& itr = currentScene.pGameObjects_.begin();
		itr != currentScene.pGameObjects_.end();)
	{
		if ((*itr)->IsToDestory())
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
