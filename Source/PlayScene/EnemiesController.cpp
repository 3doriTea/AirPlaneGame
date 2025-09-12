#include "EnemiesController.h"
#include "../Enemy.h"
#include "../TestScene/EnemyPlane.h"

using namespace mtgb;

EnemiesController::EnemiesController(const EntityId playerId_) : GameObject(GameObjectBuilder()
	.Build()),
	pPlayerTransform_{ &Transform::Get(playerId_) },
	playerPosition_{},
	playerId_{ playerId_ }
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
	Instantiate<EnemyPlane>(_worldPosition, playerId_, GetEntityId());
	//Instantiate<Enemy>(_worldPosition, GetEntityId());
}
