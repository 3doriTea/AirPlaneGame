#include "EnemiesController.h"
#include "../Enemy.h"
#include "../TestScene/EnemyPlane.h"

using namespace mtgb;

namespace
{
	const float SPAWAN_OFFSET_Y{ 100.0f };  // ƒXƒ|[ƒ“‚·‚é‚‚³y
}

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
	Instantiate<EnemyPlane>(_worldPosition + Vector3::Up() * SPAWAN_OFFSET_Y, playerId_, GetEntityId());
	//Instantiate<Enemy>(_worldPosition, GetEntityId());
}
