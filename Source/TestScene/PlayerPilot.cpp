#include "PlayerPilot.h"
#include "PlayerBullet.h"

using namespace mtgb;

PlayerPilot::PlayerPilot(const EntityId _plane) : GameObject(GameObjectBuilder()
	.Build()),
	pTransform{ Component<Transform>() },
	lockOnSide_{400}
{
	Vector2Int screenSize = Game::System<Screen>().GetSize();

	rectCenter_.x = screenSize.x / 2;
	rectCenter_.y = screenSize.y / 2;

	lockOnRect_.x = rectCenter_.x - lockOnSide_ / 2;
	lockOnRect_.width = lockOnSide_;
	lockOnRect_.y = rectCenter_.y - lockOnSide_ / 2;
	lockOnRect_.height = lockOnSide_;

	pTransform->SetParent(_plane);
	hImage_ = Image::Load("Image/lockOnFrame.png");
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
	const Vector2Int DRAW_SIZE{ lockOnSide_ ,lockOnSide_ };
	Draw::Image(hImage_, lockOnRect_, { Vector2Int::Zero(),Image::GetSize(hImage_) }, 0.0f);
}

void PlayerPilot::LockOnShoot()
{
	static std::vector<GameObject*> enemies;
	// TODO : WindowContextÇÇ◊ÇΩèëÇ´Ç≈Ç»Ç≠PlayerPilotÇ™é©êgÇÃÇï€éùÇ∑ÇÈÇÊÇ§Ç…!!!
	Game::System<ColliderCP>().RectContains(lockOnRect_, "Enemy", &enemies, WindowContext::First);
	if (enemies.empty())
		return;

	// ÉèÅ[ÉãÉhç¿ïWånÇ≈àÍî‘ãﬂÇ¢ìGÇë_Ç§
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
	Instantiate<PlayerBullet>(pTransform->position + Vector3::Forward() * 1.0f, shootDir);


}
