#include "EnemiesController.h"
#include "../Enemy.h"
#include "../TestScene/EnemyPlane.h"

using namespace mtgb;

namespace
{
	const float SPAWAN_OFFSET_Y{ 100.0f };  // スポーンする高さy
	const int TELEPORT_ENEMY_COUNT{ 2 };  // テレポートさせる敵の数
	const float TELEPORT_PLAYER_DISTANCE{ 140.0f };
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

void EnemiesController::TeleportEnemy()
{
	std::vector<EnemyPlane*> founds{};
	FindGameObjects<EnemyPlane>(&founds);

	for (int i = 0; i < founds.size() && i < TELEPORT_ENEMY_COUNT; i++)
	{
		if (founds[i] == nullptr)
		{
			continue;
		}
		Transform& eTrans{ Transform::Get(founds[i]->GetEntityId()) };
		Vector3 diff{ eTrans.position - playerPosition_ };
		Vector3 add{ Vector3::Normalize(diff) * TELEPORT_PLAYER_DISTANCE };
		eTrans.position = playerPosition_ + add;
	}
}
