#include "PlayerPilot.h"
#include "PlayerBullet.h"

using namespace mtgb;

PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.Build()),
	pTransform{ Component<Transform>() },
	rockOnSide_{400}
{
	Vector2Int screenSize = Game::System<Screen>().GetSize();

	rectCenter_.x = screenSize.x / 2;
	rectCenter_.y = screenSize.y / 2;

	rockOnRect_.x = rectCenter_.x - rockOnSide_ / 2;
	rockOnRect_.width = rockOnSide_;
	rockOnRect_.y = rectCenter_.y - rockOnSide_ / 2;
	rockOnRect_.height = rockOnSide_;

	pTransform->SetParent(_plane);
}

PlayerPilot::~PlayerPilot()
{
}

void PlayerPilot::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::Space))
	{
		LOGIMGUI("Pilot:shoot");
		LockOnShoot();
	}

	MTImGui::Instance().TypedShow(pTransform, "PlayerPilot");
}

void PlayerPilot::Draw() const
{
}

void PlayerPilot::LockOnShoot()
{
	static std::vector<GameObject*> enemies;
	// TODO : WindowContextをべた書きでなくPlayerPilotが自身のを保持するように!!!
	Game::System<ColliderCP>().RectContains(rockOnRect_, "Enemy", &enemies, WindowContext::First);
	if (enemies.empty())
		return;

	// ワールド座標系で一番近い敵を狙う
	GameObject* nearestEnemy = enemies.front();
	for (auto& enemy : enemies)
	{
		float enemyDis = (pTransform->position - enemy->Component<Transform>()->position).Size();
		float nearestDis = (pTransform->position - nearestEnemy->Component<Transform>()->position).Size();

		if (enemyDis < nearestDis)
		{
			nearestEnemy = enemy;
		}
	}

	Vector3 targetPos = nearestEnemy->Component<Transform>()->position;
	Vector3 toTarget = Vector3::Normalize(targetPos - pTransform->position);
	Quaternion shootDir = Quaternion::LookRotation(toTarget, Vector3::Up());
	Instantiate<PlayerBullet>(pTransform->position + Vector3::Down() * 1.0f, shootDir);

}
