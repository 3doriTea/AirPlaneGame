#include "ControlTower.h"

ControlTower::ControlTower(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("ControlTower")
	.Build())
	,detectionRagius_{30.0f}
{
	bool getPlayerPlaneTransform = Game::System<TransformCP>().TryGet(pPlayerPlaneTransform_, _plane);
	if (!getPlayerPlaneTransform)
	{
		LOGIMGUI("Failed to get PlayerPlaneTransform!!!!!");
	}

}

ControlTower::~ControlTower()
{
}

void ControlTower::Update()
{
	DetectionEnemy();
}

void ControlTower::Draw() const
{
}

void ControlTower::DetectionEnemy()
{

}
