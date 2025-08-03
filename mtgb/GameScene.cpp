#include "GameScene.h"
#include "SceneSystem.h"
#include "GameObject.h"
#include "Transform.h"
#include "CameraSystem.h"

mtgb::GameScene::GameScene()
{
}

mtgb::GameScene::~GameScene()
{
}

mtgb::CameraHandleInScene mtgb::GameScene::RegisterCameraGameObject(GameObject* _pGameObject) const
{
	Transform* pTransform{ &Transform::Get(_pGameObject->GetEntityId()) };
	return Game::System<CameraSystem>().RegisterDrawCamera(pTransform);
}

void mtgb::GameScene::Initialize()
{
}

void mtgb::GameScene::Update()
{
}

void mtgb::GameScene::Draw() const
{
}

void mtgb::GameScene::End()
{
}

mtgb::GameScene* mtgb::GameScene::pInstance_{ nullptr };
