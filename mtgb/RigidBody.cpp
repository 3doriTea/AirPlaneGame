#include "RigidBody.h"
#include "Transform.h"

mtgb::RigidBody::RigidBody(const EntityId _entityId) :
	IComponent{ _entityId },
	pTransform_{ &Transform::Get(_entityId) }
{
}

mtgb::RigidBody::~RigidBody()
{
}
