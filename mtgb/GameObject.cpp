#include "GameObject.h"
#include "SceneSystem.h"
#include "Transform.h"


mtgb::GameObject::GameObject(const GAME_OBJECT_DESC& _desc) :
	name_{_desc.name},
	status_
	{
		.isActive_ = _desc.isActive,
		.callUpdate_ = _desc.callUpdate,
		.callDraw_ = _desc.callDraw,
		.toDestroy_ = FALSE,  // ç≈èâÇÕçÌèúÇµÇ»Ç¢
	},
	layerFlag_{ _desc.layerFlag }
{
	Transform* pTransform_{ Component<Transform>() };
	pTransform_->position = _desc.position;
	pTransform_->rotate = _desc.rotate;
	pTransform_->scale = _desc.scale;
}

mtgb::GameObject::GameObject(const GameObject& _other)
	:Entity()
	,status_{_other.status_}
	,componentsFlag_{_other.componentsFlag_}
{
}

mtgb::GameObject::~GameObject()
{
}

mtgb::GameObject* mtgb::GameObject::FindGameObject(const std::string& _name)
{
	return mtgb::Game::System<SceneSystem>().GetActiveScene()->GetGameObject(_name);
}
