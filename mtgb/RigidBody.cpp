#include "RigidBody.h"
#include "Transform.h"

mtgb::RigidBody::RigidBody(const EntityId _entityId) :
	IComponent{ _entityId },
	pTransform_{ &Transform::Get(_entityId) },
	onHit_{ [](const EntityId){} }
{
}

mtgb::RigidBody::~RigidBody()
{
}

void mtgb::RigidBody::OnCollisionEnter(const std::function<void(const EntityId)> _onHit)
{
	onHit_ = _onHit;
}
