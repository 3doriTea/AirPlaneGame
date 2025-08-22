#include "Radar.h"

namespace
{
	const int IMAGE_SIZE_PX{ 280 };
	const int ENEMY_POS_CAPACITY{ 20 };
	const int ENEMY_MARK_SIZE_PX{ 10 };
	const int MARGIN_PX{ 2 };
}

Radar::Radar(const EntityId _playerId, const GameObjectLayer _layer) : GameObject(GameObjectBuilder()
	.SetLayerFlag(_layer)
	.Build()),
	pPlayerTransform_{ &Transform::Get(_playerId) },
	viewAngle_{ 0.0f }
{
	hBack_ = Image::Load("Image/RadarBack.png");
	massert(hBack_ >= 0 && "レーダー盤画像読み込みに失敗 @Radar::Radar");
	hInView_ = Image::Load("Image/RadarInView.png");
	massert(hInView_ >= 0 && "レーダー視野画像読み込みに失敗 @Radar::Radar");
	hFrame_ = Image::Load("Image/RadarFrame.png");
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
}

void Radar::Draw() const
{
	const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	const Vector2Int RADAR_OFFSET{ SCREEN_SIZE.x - IMAGE_SIZE_PX / 2, IMAGE_SIZE_PX / 2 };

	auto drawImage
	{
		[&, this](const ImageHandle _hImage, const float _angle = 0.0f)
		{
			Draw::Image(
				_hImage,
				{ SCREEN_SIZE.x - IMAGE_SIZE_PX, 0, IMAGE_SIZE_PX, IMAGE_SIZE_PX },
				{ MARGIN_PX, MARGIN_PX, IMAGE_SIZE_PX - MARGIN_PX, IMAGE_SIZE_PX - MARGIN_PX },
				_angle);
		}
	};

	drawImage(hBack_);
	drawImage(hInView_, viewAngle_);
	drawImage(hFrame_);

	for (auto& markPos : enemyMarkPos_)
	{
		Draw::Box({ markPos + RADAR_OFFSET - (Vector2Int::One() * ENEMY_MARK_SIZE_PX / 2), Vector2Int{ENEMY_MARK_SIZE_PX, ENEMY_MARK_SIZE_PX} }, 0xff0000);
	}
}
