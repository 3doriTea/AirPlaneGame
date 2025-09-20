#include "Radar.h"
#include "../EnemyPlane.h"

namespace
{
	const int IMAGE_SIZE_PX{ 280 };
	const int ENEMY_POS_CAPACITY{ 20 };
	const int ENEMY_MARK_SIZE_PX{ 10 };
	const int MARGIN_PX{ 2 };
	// 敵マークを消す範囲
	const int HIDE_DISTANCE{ 140 };
	const int HIDE_DISTANCE_DOUBLE{ HIDE_DISTANCE * HIDE_DISTANCE };
	const Color ENEMY_BOX_COLOR = Color::RED;
	const Color MISSILE_BOX_COLOR = Color::BLACK;
}

Radar::Radar(const EntityId _playerId, const GameObjectLayer _layer) : GameObject(GameObjectBuilder()
	.SetLayerFlag(GameObjectLayerFlag::New()
		.BeginEdit()
		.On(_layer)
		.EndEdit())
	.Build()),
	pPlayerTransform_{ &Transform::Get(_playerId) },
	viewAngle_{ 0.0f }
{
	hBack_ = Image::Load("Image/RadarBack2.png");
	massert(hBack_ >= 0 && "レーダー盤画像読み込みに失敗 @Radar::Radar");
	hInView_ = Image::Load(
		_layer == GameObjectLayer::A
		? "Image/RadarInViewA.png"
		: "Image/RadarInViewB.png");
	massert(hInView_ >= 0 && "レーダー視野画像読み込みに失敗 @Radar::Radar");
	hFrame_ = Image::Load(
		_layer == GameObjectLayer::A
		? "Image/RadarFrameA.png"
		: "Image/RadarFrameB.png");
	massert(hFrame_ >= 0 && "レーダーフレーム画像読み込みに失敗 @Radar::Radar");

	//hBack_ = Image::Load("Image/RadarBase.png");
	//massert(hBack_ >= 0 && "レーダー盤画像読み込みに失敗 @Radar::Radar");

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
		EnemyPlane* pEnemy{ dynamic_cast<EnemyPlane*>(pGameObject) };

		if (pEnemy->IsActive() == false)
		{
			continue;
		}

		Transform& enemyTransform{ Transform::Get(pGameObject->GetEntityId()) };
		//Vector3 diff{ enemyTransform.position - pPlayerTransform_->GetWorldPosition() };
		Vector3 diff{ enemyTransform.position };
		Matrix4x4 mPlayerWorld{};
		pPlayerTransform_->GenerateWorldMatrix(&mPlayerWorld);
		mPlayerWorld = DirectX::XMMatrixInverse(nullptr, mPlayerWorld);
		diff *= mPlayerWorld;
		//Vector3 diff{ enemyTransform.position * mPlayerWorld };

		enemyMarkPos_.emplace_back(static_cast<int>(diff.x), -static_cast<int>(diff.z));
	}

	std::vector<GameObject*> pMissiles{};
	FindGameObjects("Missile", &pMissiles);

	missileMarkPos_.clear();
	for (auto& pGameObject : pMissiles)
	{
		Transform& missileTransform{ Transform::Get(pGameObject->GetEntityId()) };
		Vector3 diff{ missileTransform.position };
		Matrix4x4 mPlayerWorld{};
		pPlayerTransform_->GenerateWorldMatrix(&mPlayerWorld);
		mPlayerWorld = DirectX::XMMatrixInverse(nullptr, mPlayerWorld);
		diff *= mPlayerWorld;

		missileMarkPos_.emplace_back(static_cast<int>(diff.x), -static_cast<int>(diff.z));
	}
}

void Radar::Draw() const
{
	const Vector2F SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	const Vector2F RADAR_OFFSET{ SCREEN_SIZE.x - IMAGE_SIZE_PX / 2, IMAGE_SIZE_PX / 2 };
	const int DEPTH_OFFSET{ GetLayerFlag().Has(GameObjectLayer::A) ? 10 : 0 };

	auto drawImage
	{
		[&, this](const ImageHandle _hImage, const int _layer, const float _angle = 0.0f)
		{
			Draw::Image(
				_hImage,
				{ SCREEN_SIZE.x - IMAGE_SIZE_PX, 0, IMAGE_SIZE_PX, IMAGE_SIZE_PX },
				{ MARGIN_PX, MARGIN_PX, IMAGE_SIZE_PX - MARGIN_PX, IMAGE_SIZE_PX - MARGIN_PX },
				_angle, {.layerFlag = layerFlag_ });
		}
	};

	drawImage(hBack_, 0);
	drawImage(hInView_, 0, viewAngle_);
	drawImage(hFrame_, 1);

	for (Vector2Int markPos : enemyMarkPos_)
	{
		// 距離がレーダー範囲外なら端っこに描画
		int lengthDouble{ markPos.x * markPos.x + markPos.y * markPos.y };
		if (lengthDouble >= HIDE_DISTANCE_DOUBLE)
		{
			float length{ std::sqrtf(static_cast<float>(lengthDouble)) };
			float x = markPos.x / length;
			float y = markPos.y / length;
			markPos.x = x * HIDE_DISTANCE;
			markPos.y = y * HIDE_DISTANCE;
		}
		Draw::Box({ markPos + RADAR_OFFSET - (Vector2Int::One() * ENEMY_MARK_SIZE_PX / 2), Vector2Int{ENEMY_MARK_SIZE_PX, ENEMY_MARK_SIZE_PX} }, ENEMY_BOX_COLOR, { .depth = DEPTH_OFFSET + 1,.layerFlag = layerFlag_ });
	}

	for (auto& markPos : missileMarkPos_)
	{
		// 距離がレーダー範囲外なら描画しない
		if (markPos.x * markPos.x + markPos.y * markPos.y >= HIDE_DISTANCE_DOUBLE)
		{
			continue;
		}
		Draw::Box({ markPos + RADAR_OFFSET - (Vector2Int::One() * ENEMY_MARK_SIZE_PX / 2), Vector2Int{ENEMY_MARK_SIZE_PX, ENEMY_MARK_SIZE_PX} }, MISSILE_BOX_COLOR, { .depth = DEPTH_OFFSET + 1,.layerFlag = layerFlag_ });
	}
}
