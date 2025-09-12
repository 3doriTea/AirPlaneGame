#include "EnemiesController.h"
#include "../Enemy.h"

using namespace mtgb;

EnemiesController::EnemiesController(const EntityId playerId_) : GameObject(GameObjectBuilder()
	.Build()),
	pPlayerTransform_{ &Transform::Get(playerId_) },
	playerPosition_{}
{
}

EnemiesController::~EnemiesController()
{
}

void EnemiesController::Update()
{
	playerPosition_ = pPlayerTransform_->GetWorldPosition();
}

void EnemiesController::Spawan(const Vector3 _worldPosition)
{
	Instantiate<Enemy>(_worldPosition, GetEntityId());
}
