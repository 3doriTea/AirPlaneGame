#include "RigidBody.h"
#include "Transform.h"

mtgb::RigidBody::RigidBody(const EntityId _entityId) :
	IComponent{ _entityId },
	pTransform_{ &Transform::Get(_entityId) },
	onHit_{ [](const EntityId){} },
	isNeedUpdate_{ false }
{
}

mtgb::RigidBody::~RigidBody()
{
}

void mtgb::RigidBody::OnCollisionEnter(const std::function<void(const EntityId)>& _onHit)
{
	isNeedUpdate_ = true;
	onHit_ = _onHit;
}
