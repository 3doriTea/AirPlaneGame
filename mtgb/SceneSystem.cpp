#include "SceneSystem.h"
#include "GameObject.h"

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

	// 現在のシーン
	GameScene& currentScene{ *GameScene::pInstance_ };

	// 更新処理
	currentScene.Update();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Update();
	}

	// 描画処理
	currentScene.Draw();
	for (auto&& gameObject : currentScene.pGameObjects_)
	{
		gameObject->Draw();
	}

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
