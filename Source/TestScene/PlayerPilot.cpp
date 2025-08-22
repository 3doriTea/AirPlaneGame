#include "PlayerPilot.h"

using namespace mtgb;

PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })

	.Build()),
	pTransform_{ Component<Transform>() }
{
	pTransform_->SetParent(_plane);
}

PlayerPilot::~PlayerPilot()
{
}

void PlayerPilot::Update()
{
	Vector3 worldPos{ pTransform_->GetWorldPosition() };
	LOGF("P:Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);
}

void PlayerPilot::Draw() const
{
}
