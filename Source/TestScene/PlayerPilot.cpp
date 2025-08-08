#include "PlayerPilot.h"

using namespace mtgb;

PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.Build()),
	pTransform{ Component<Transform>() }
{
	pTransform->SetParent(_plane);
}

PlayerPilot::~PlayerPilot()
{
}

void PlayerPilot::Update()
{
}

void PlayerPilot::Draw() const
{
}
