#include "ControlTower.h"
#include "TestScene/EnemyPlane.h"
#include "MTStringUtility.h"
#include <format>
ControlTower::ControlTower() : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("ControlTower")
	.Build())
	,detectionRadius_{30.0f}
	
{
	// 初期化
	pGunner_ = { nullptr,WindowContext::First };
	pPilot_ = { nullptr,WindowContext::Second };

	Vector2F screenSize = Game::System<Screen>().GetSizeF();
	// 画面上の敵を検出する距離
	float detectDistance = Game::System<CameraSystem>().GetFar();

	// ターゲットの強調表示の画像、画像サイズ
	highlightFrameImage_ = Image::Load("Image/highlightEnemyFrame.png");
	enemyArrowImage_ = Image::Load("Image/enemyArrow.png");
	highlightFrameSize_ = { 60.0f,60.0f };
	enemyArrowImageSize_ = { 30.0f,30.0f };
	
}

ControlTower::~ControlTower()
{
}

void ControlTower::Update()
{
	for (auto& detector : wndRectDetector_)
	{
		detector.second.UpdateDetection();
	}
	if (!controlTargetTransform_.empty())
	{
		// 現状二人のプレイヤーは同じ飛行機に乗っていて座標は同じなので先頭のTransformを渡す
		DetectionEnemy(controlTargetTransform_.begin()->second);
	}
	Transform* pCameraTransform = controlTargetTransform_[WindowContext::First];

	// 敵の Transformを取得
	if (attackStateEnemies_.empty()) return;

	Transform& enemyTransform = Transform::Get(attackStateEnemies_[0]);

	// カメラから敵への方向ベクトルを計算
	Vector3 toEnemy = Vector3::Normalize(enemyTransform.GetWorldPosition() - pCameraTransform->GetWorldPosition());

	// カメラ視点の敵の方向を2D座標系で計算
	float x = DirectX::XMVectorGetX(DirectX::XMVector3Dot(toEnemy, pCameraTransform->Right()));
	float y = DirectX::XMVectorGetX(DirectX::XMVector3Dot(toEnemy, pCameraTransform->Up()));

	// ウィンドウの中心から円周上の位置を計算
	Vector2F screenCenter = Game::System<Screen>().GetSizeF() * 0.5f;
	float circleRadius = 100.0f; // 矢印を表示する円の半径(仮)

	// 敵の方向の角度を計算
	float angle = DirectX::XMConvertToDegrees( std::atan2f(y, x));
	MTImGui::Instance().DirectShow([angle,x,y]()
		{
			ImGui::Text("x,y:%.3f,%.3f", x,y);
			ImGui::Text("angle:%.3f", angle);
		},"angle",ShowType::Inspector);
}

void ControlTower::Draw() const
{
	if (auto itr = wndRectDetector_.find(CurrContext()); itr != wndRectDetector_.end())
	{
		const RectDetector& detector = itr->second;

		std::vector<EntityId> outOfScreenTargets;
		// 攻撃状態の敵
		for (EntityId enemy : attackStateEnemies_)
		{
			// 画面上に攻撃状態の敵がいるか確認
			auto& detectedTargets = detector.detectedTargets;
			bool isOnScreen = std::any_of(detectedTargets.begin(), detectedTargets.end(),
				[enemy](const RectContainsInfo& _info)
				{
					return _info.entityId == enemy;
				});

			// 画面外の敵のIDを保存
			if (!isOnScreen)
			{
				outOfScreenTargets.push_back(enemy);
			}
		}
		
		// 画面外の敵の描画
		for (EntityId outOfScreenTarget : outOfScreenTargets)
		{
			DrawEnemyArrow(outOfScreenTarget);
		}
	}
}



void ControlTower::SetControlTarget(EntityId _id, WindowContext _context)
{
	if (controlTargetTransform_.contains(_context) == false)
	{
		controlTargetTransform_[_context] = &Transform::Get(_id);
	}
	if (wndRectDetector_.contains(_context) == false)
	{
		Vector2F screenSize = Game::System<Screen>().GetSizeF();
		// 画面上の敵を検出する距離
		float detectDistance = Game::System<CameraSystem>().GetFar();

		RectDetectorConfig config =
		{
			.targetName = "Enemy",
			.windowContext = _context,
			.detectionRect =
			{
				0.0f,
				0.0f,
				static_cast<float>(screenSize.x),
				static_cast<float>(screenSize.y),
			},
			.maxDistance = detectDistance,
		};
		wndRectDetector_.try_emplace(_context, config);
	}
}

void ControlTower::DetectionEnemy(Transform* _transform)
{
	// Enemyを取得
	std::vector<EnemyPlane*> enemies;
	FindGameObjects<EnemyPlane>(&enemies);

	// プレイヤーに対して戦闘を行う状態になっている敵のみにする
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(),
			[&](const EnemyPlane* _enemy) 
			{
				auto& mainState = _enemy->GetAI().GetMainState();
				auto& fightState = _enemy->GetAI().GetFightState();
				EnemyAI::MAIN_STATE currState = mainState.Current();
				// 戦闘状態でないならば trueにして破棄
				if (currState != EnemyAI::MAIN_STATE::S_FIGHT)
				{
					return true;
				}
				else
				{
					return false;
				}
				/*if (fightState.Current() == EnemyAI::FIGHT_STATE::SF_LOOK_AT_PLAYER)
				{

				}*/
			}),
		enemies.end()
		);

	// 敵がいないなら
	if (enemies.empty())
	{
		return;
	}

	// 先頭の敵のEntityIdを取得
	EntityId enemyId = enemies.front()->GetEntityId();

	// 敵が二体以上いるなら一番近くのを選ぶ
	if (enemies.size() >= 2)
	{
		// 一番近くの敵を取得
		auto itr =std::min_element(
			enemies.begin(),
			enemies.end(),
			[this,_transform](EnemyPlane* a, EnemyPlane* b)
			{
				float distanceA = (_transform->GetWorldPosition() - Transform::Get(a->GetEntityId()).position).Size();
				float distanceB = (_transform->GetWorldPosition() - Transform::Get(b->GetEntityId()).position).Size();
				return distanceA < distanceB;
			}
		);
		enemyId = (*itr)->GetEntityId();
	}

	attackStateEnemies_.clear();
	attackStateEnemies_.push_back(enemyId);
	// 方角を計算
	// プレイヤーの上ベクトル、右ベクトル
	Vector3 up = _transform->Up();
	Vector3 right = _transform->Right();
	Vector3 forward = _transform->Forward();
	
	Transform& enemy = Transform::Get(enemyId);
	
	
	// 敵のスクリーン座標を取得
	//Game::System<CameraSystem>().Get
	//for (const auto& enemy : enemies)
	//{
	//	Transform& enemyTransform = Transform::Get(enemy->GetEntityId());
	//	Vector3 toEnemy = Vector3::Normalize(enemyTransform.position - _transform->position);

	//	std::string str1= "", str2 = "",str3 = "";
	//	float horizontal = DirectX::XMVector3Dot((toEnemy), right).m128_f32[0];
	//	// 右
	//	if (horizontal == 0)
	//	{
	//		//str1 = "正面";
	//	}
	//	else if (horizontal > 0)
	//	{
	//		str1 = "右";
	//	}
	//	// 左
	//	else
	//	{
	//		str2 = "左";
	//	}
	//	
	//	float vertical = DirectX::XMVector3Dot(toEnemy, up).m128_f32[0];
	//	// 上
	//	if (vertical == 0)
	//	{
	//		//str2 = "正面";
	//	}
	//	else if (vertical > 0)
	//	{
	//		str2 = "上";
	//	}
	//	else
	//	{
	//		str2 = "下";
	//	}

	//	float upOrBack = DirectX::XMVector3Dot(toEnemy,forward ).m128_f32[0];
	//	// 正面
	//	if (upOrBack > 0)
	//	{
	//		str3 = "前方";
	//	}
	//	else if(upOrBack < 0)
	//	{
	//		str3 = "後ろ";
	//	}
	//	else if (upOrBack == 0)
	//	{
	//		str3 = "真横";
	//	}
	//	std::string str4 = "";
	//	
	//	LOGIMGUI("Camera%d,Enemy%lld:%s,%s,%s",_hCamera, enemy->GetEntityId(),str1.c_str(), str2.c_str(), str3.c_str());
	//	
	//	ret = std::format("{}:{},{},{}\n", enemy->GetEntityId(), str1, str2, str3);
	//}
	//return ret;
}

void ControlTower::DrawEnemyArrow(EntityId _entityId) const
{
	// 現在のカメラ(プレイヤー)の Transformを取得

	WindowContext context = CurrContext();
	if (controlTargetTransform_.contains(context) == false) return;
	Transform* pCameraTransform = controlTargetTransform_.find(context)->second;
	if (!pCameraTransform) return;

	// 敵の Transformを取得
	Transform& enemyTransform = Transform::Get(_entityId);

	// カメラから敵への方向ベクトルを計算
	Vector3 toEnemy = Vector3::Normalize(enemyTransform.GetWorldPosition() - pCameraTransform->GetWorldPosition());

	// カメラ視点の敵の方向を2D座標系で計算
	float x = DirectX::XMVectorGetX(DirectX::XMVector3Dot(toEnemy, pCameraTransform->Right()));
	float y = DirectX::XMVectorGetX(DirectX::XMVector3Dot(toEnemy, pCameraTransform->Up()));

	// ウィンドウの中心から円周上の位置を計算
	Vector2F screenCenter = Game::System<Screen>().GetSizeF() * 0.5f;
	float circleRadius = 100.0f; // 矢印を表示する円の半径(仮)

	// 敵の方向の角度を計算
	float angle = std::atan2f(y, x);
	// 角度を元に円周上に配置
	Vector2F arrowPos =
	{
		screenCenter.x + (std::cosf(angle) * circleRadius),
		screenCenter.y - (std::sinf(angle) * circleRadius)
	};

	// 矢印を描画
	// 画像を回転させるときは逆向きに回転させる
	DrawArrowAtPosition(arrowPos, -angle);
}

void ControlTower::DrawArrowAtPosition(const Vector2F& _position, float _angle) const
{
	// 矢印の画像を回転させて描画
	RectF drawRect =
	{
		_position.x - (enemyArrowImageSize_.x  * 0.5f),
		_position.y - (enemyArrowImageSize_.y  * 0.5f),
		enemyArrowImageSize_.x,
		enemyArrowImageSize_.y
	};

	Draw::Image(enemyArrowImage_, drawRect, { Vector2F::Zero(),Image::GetSizeF(enemyArrowImage_) }, _angle);
	//Draw::Image(enemyArrowImage_, drawRect);
}
