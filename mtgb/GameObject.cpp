#include "GameObject.h"
#include "SceneSystem.h"
#include "Transform.h"


mtgb::GameObject::GameObject(const GAME_OBJECT_DESC& _desc) :
	status_
	{
		.isActive = _desc.isActive,
		.callUpdate_ = _desc.callUpdate,
		.callDraw_ = _desc.callDraw,
		.toDestory_ = FALSE,  // ç≈èâÇÕçÌèúÇµÇ»Ç¢
	}
{
	Transform* pTransform_{ Component<Transform>() };
	pTransform_->position_ = _desc.position;
	pTransform_->rotate_ = _desc.rotate;
	pTransform_->scale_ = _desc.scale;
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
