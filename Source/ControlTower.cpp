#include "ControlTower.h"
#include "Enemy.h"
ControlTower::ControlTower(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("ControlTower")
	.Build())
	,detectionRagius_{30.0f}
{
	pPlayerPlaneTransform_ = Component<Transform>();
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
	//std::vector<Enemy*> enemies
}
