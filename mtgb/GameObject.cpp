#include "GameObject.h"
#include "SceneSystem.h"


mtgb::GameObject::GameObject(const GAME_OBJECT_DESC& _desc) :
	status_
	{
		.isActive = _desc.isActive,
		.callUpdate_ = _desc.callUpdate,
		.callDraw_ = _desc.callDraw,
		.toDestory_ = FALSE,  // ç≈èâÇÕçÌèúÇµÇ»Ç¢
	}
{
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
