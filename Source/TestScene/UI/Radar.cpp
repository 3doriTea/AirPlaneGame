#include "Radar.h"

namespace
{
	const int IMAGE_SIZE_PX{ 280 };
	const int ENEMY_POS_CAPACITY{ 20 };
	const int ENEMY_MARK_SIZE_PIX{ 10 };
}

Radar::Radar(const EntityId _playerId) : GameObject(GameObjectBuilder()
	.Build()),
	pPlayerTransform_{ &Transform::Get(_playerId) }
{
	hBack_ = Image::Load("Image/RadarBase.png");
	massert(hBack_ >= 0 && "ÉåÅ[É_Å[î’âÊëúì«Ç›çûÇ›Ç…é∏îs @Radar::Radar");

	enemyMarkPos_.reserve(ENEMY_POS_CAPACITY);
}

Radar::~Radar()
{
}

void Radar::Update()
{
	std::vector<GameObject*> pEnemies{};
	FindGameObjects("Enemy", &pEnemies);

	enemyMarkPos_.clear();
	for (auto& pGameObject : pEnemies)
	{
		Transform& enemyTransform{ Transform::Get(pGameObject->GetEntityId()) };
		Vector3 diff{ pPlayerTransform_->position - enemyTransform.position };
		Matrix4x4 mPlayerWorld{};
		pPlayerTransform_->GenerateWorldMatrix(&mPlayerWorld);
		diff *= mPlayerWorld;
		//Vector3 diff{ enemyTransform.position * mPlayerWorld };

		enemyMarkPos_.emplace_back(static_cast<int>(diff.x), static_cast<int>(diff.z));
	}
}

void Radar::Draw() const
{
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	const Vector2Int RADAR_OFFSET{ SCREEN_SIZE.x - IMAGE_SIZE_PX / 2, IMAGE_SIZE_PX / 2 };

	Draw::Image(
		hBack_,
		{ SCREEN_SIZE.x - IMAGE_SIZE_PX, 0, IMAGE_SIZE_PX, IMAGE_SIZE_PX },
		{ 2, 2, IMAGE_SIZE_PX - 2, IMAGE_SIZE_PX - 2 },
		0.0f);

	for (auto& markPos : enemyMarkPos_)
	{
		Draw::Box({ markPos + RADAR_OFFSET - (Vector2Int::One() * ENEMY_MARK_SIZE_PIX / 2), Vector2Int{ENEMY_MARK_SIZE_PIX, ENEMY_MARK_SIZE_PIX} }, 0xff0000);
	}
}
