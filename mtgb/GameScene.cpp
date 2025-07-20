#include "GameScene.h"
#include "SceneSystem.h"
#include "GameObject.h"
#include "Transform.h"

mtgb::GameScene::GameScene() :
	pCameraGameObject_{ nullptr }
{
}

mtgb::GameScene::~GameScene()
{
}

void mtgb::GameScene::SetCameraGameObject(GameObject* _pGameObject)
{
	pCameraGameObject_ = _pGameObject;
	pCameraTransform_ = &Transform::Get(_pGameObject->GetEntityId());
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
