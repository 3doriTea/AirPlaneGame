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

mtgb::GameObject* mtgb::GameScene::GetGameObject(std::string _name) const
{
	for (auto& object : pGameObjects_)
	{
		if (object->GetName() != _name)
		{
			continue;
		}
		return object;
	}
	return nullptr;
}

mtgb::GameObject* mtgb::GameScene::GetGameObject(const EntityId _entityId) const
{
	for (auto& object : pGameObjects_)
	{
		if (object->GetEntityId() != _entityId)
		{
			continue;
		}
		return object;
	}

	return nullptr;
}

mtgb::GameScene* mtgb::GameScene::pInstance_{ nullptr };
