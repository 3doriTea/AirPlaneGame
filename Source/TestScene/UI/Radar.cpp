#include "Radar.h"
#include "../EnemyPlane.h"
#include "../../PlayScene/EnemiesController.h"

namespace
{
	const int IMAGE_SIZE_PX{ 280 };
	const int ENEMY_POS_CAPACITY{ 20 };
	const int ENEMY_MARK_SIZE_PX{ 20 };
	const int MARGIN_PX{ 2 };
	// 敵マークを端っこに表示する範囲
	const int CLAMP_DISTANCE{ 140 };
	const int CLAMP_DISTANCE_SQUARED{ CLAMP_DISTANCE * CLAMP_DISTANCE };
	// 敵マークを消す範囲
	const int HIDE_DISTANCE{ 200 };
	const int HIDE_DISTANCE_SQUARED{ HIDE_DISTANCE * HIDE_DISTANCE };
	const Color ENEMY_BOX_COLOR = Color::RED;
	const Color MISSILE_BOX_COLOR = Color::BLACK;

	void ToMark2D(const Transform& _origin, const std::vector<GameObject*>& _pGameObjs, std::vector<Radar::Mark2D>* _mark2Ds)
	{
		Matrix4x4 mOriginWorld{};
		_origin.GenerateWorldMatrix(&mOriginWorld);
		Matrix4x4 mInvOrigin{ DirectX::XMMatrixInverse(nullptr,mOriginWorld) };

		Matrix4x4 mOriginRot{};
		_origin.GenerateWorldRotationMatrix(&mOriginRot);
		Matrix4x4 mInvOriginRot{ DirectX::XMMatrixInverse(nullptr,mOriginRot) };

		for (auto& obj : _pGameObjs)
		{
			Transform& transform{ Transform::Get(obj->GetEntityId()) };
			Vector3 pos{ transform.position };

			
			pos *= mInvOrigin;

			Vector2Int pos2d{ static_cast<int>(pos.x), -static_cast<int>(pos.z) };

			Vector3 forward = transform.Forward() * mInvOriginRot;
			float angle = std::atan2f(forward.z, forward.x);

			_mark2Ds->push_back(Radar::Mark2D{ .pos = pos2d,.angle = angle });
		}
	}
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
	hEnemyArrow_ = Image::Load("Image/RedRadarArrow.png");
	hMissileArrow_ = Image::Load("Image/BlackRadarArrow.png");

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

	bool isShowEnemy{ false };
	for (auto pEnemy : pEnemies)
	{
		Transform& eTrans{ Transform::Get(pEnemy->GetEntityId()) };
		/*Vector3 eneWorldPos = eTrans.GetWorldPosition();
		Vector3 playerWorldPos = pPlayerTransform_->GetWorldPosition();
		float distance = (eneWorldPos - playerWorldPos).Size();*/
		if ((eTrans.GetWorldPosition() - pPlayerTransform_->GetWorldPosition()).Size() <= CLAMP_DISTANCE)
		{
			isShowEnemy = true;
			break;
		}
	}

	// 敵が一体も表示されてなかった時の処理
	if (isShowEnemy == false)
	{
		EnemiesController* pECon{ FindGameObject<EnemiesController>() };
		if (pECon != nullptr)
		{
			pECon->TeleportEnemy();
		}
	}

	enemyMarkPos_.clear();
	ToMark2D(*pPlayerTransform_, pEnemies, &enemyMarkPos_);

	std::vector<GameObject*> pMissiles{};
	FindGameObjects("Missile", &pMissiles);

	missileMarkPos_.clear();
	ToMark2D(*pPlayerTransform_, pMissiles, &missileMarkPos_);

}

void Radar::Draw() const
{
	const Vector2F SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	const Vector2F RADAR_OFFSET{ SCREEN_SIZE.x - IMAGE_SIZE_PX / 2, IMAGE_SIZE_PX / 2 };
	
	auto drawImage
	{
		[&, this](const ImageHandle _hImage, const int _layer, const float _angle = 0.0f)
		{
			Draw::Image(
				_hImage,
				{ SCREEN_SIZE.x - IMAGE_SIZE_PX, 0, IMAGE_SIZE_PX, IMAGE_SIZE_PX },
				{ MARGIN_PX, MARGIN_PX, IMAGE_SIZE_PX - MARGIN_PX, IMAGE_SIZE_PX - MARGIN_PX },
				-_angle, UIParams{.depth = _layer,.layerFlag = layerFlag_});
		}
	};

	drawImage(hBack_, 0);
	drawImage(hInView_, 0, viewAngle_);
	drawImage(hFrame_, 1);

	
	auto drawMarks
	{
		[&,this](const std::vector<Mark2D>& _marks,ImageHandle _hArrow, int _layer)
		{
			for (const auto& mark : _marks)
			{
				float x = static_cast<float>(mark.pos.x);
				float y = static_cast<float>(mark.pos.y);
				RectF rect =
				{
					x  - ENEMY_MARK_SIZE_PX * 0.5f,
					y  - ENEMY_MARK_SIZE_PX * 0.5f,
					ENEMY_MARK_SIZE_PX,
					ENEMY_MARK_SIZE_PX
				};

				// 距離がレーダー範囲外なら端っこに描画
				int distanceSquared{ mark.pos.x * mark.pos.x + mark.pos.y * mark.pos.y };
				float distance{ std::sqrtf(static_cast<float>(distanceSquared)) };

				//if (distanceSquared >= HIDE_DISTANCE_SQUARED)
					//continue;

				if (distanceSquared >= CLAMP_DISTANCE_SQUARED)
				{
					rect.x = (rect.x / distance) * CLAMP_DISTANCE;
					rect.y = (rect.y / distance) * CLAMP_DISTANCE;
				}
				
				rect.point += RADAR_OFFSET;

				// 回転角度は反転させる
				Draw::Image(_hArrow, rect, { Vector2F::Zero(),Image::GetSizeF(_hArrow) }, -(mark.angle),UIParams{.depth = _layer,.layerFlag = layerFlag_});
			}
		}
	};

	// 敵とミサイルの描画
	drawMarks(enemyMarkPos_, hEnemyArrow_,2);
	drawMarks(missileMarkPos_, hMissileArrow_,2);
}
