#include "ControlTower.h"
#include "TestScene/EnemyPlane.h"
#include "MTStringUtility.h"
#include <format>
#include "Debug.h"

namespace
{
	SpeechLines speechLinesOnFiredMissile = 
	{
		{
			.text_ = u8"ミサイル接近!逃げて!",
			.audioFile_ = "Sound/Voice/012_ずんだもん（ノーマル）_ミサイル接近!逃げ….wav",
			.time_ = 3.0f
		},
		{
			.text_ = u8"ミサイル接近!撃ち落として!",
			.audioFile_ = "Sound/Voice/016_ずんだもん（ノーマル）_ミサイル接近!撃ち….wav",
			.time_ = 3.5f
		},
	};
	SpeechLines speechLinesOnHit;


	// ミサイル発射された時、その1
	/*SPEECH_ELEMENT speechOnFiredMissile1 =
	{
		.text_ = u8"ミサイル接近!逃げて!",
		.audioFile_ = "Sound/Voice/012_ずんだもん（ノーマル）_ミサイル接近!逃げ….wav",
		.time_ = 3.0f
	};
	SPEECH_ELEMENT speechOnFiredMissile2 =
	{
		.text_ = u8"ミサイル接近!撃ち落として!",
		.audioFile_ = "Sound/Voice/016_ずんだもん（ノーマル）_ミサイル接近!撃ち….wav",
		.time_ = 3.0f
	};*/
	

	// 被弾時、その1
	SPEECH_ELEMENT speechOnHit1 =
	{
		.text_ = u8"被弾した!",
		.audioFile_  = "Sound/Voice/013_ずんだもん（ノーマル）_被弾した!.wav",
		.time_ = 2.0f
	};

	// 字幕の表示位置
	const Vector2F SPEECH_TEXT_POS{ 620, 820 };
	// 字幕のフォントサイズ
	const int SPEECH_TEXT_SIZE{ 48 };

	const RectF WARNING_IMAGE_RECT{ 830,190,260,60 };
	

	const float BLINK_INTERVAL{ 0.5f };
}
ControlTower::ControlTower() : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("ControlTower")
	.Build())
	,detectionRadius_{30.0f}
	, currentThreatLevel_{ThreatLevel::Normal}
{
	warningImage_ = Image::Load("Image/Warning.png");
	pWarningBlinker_ = new ImageBlinker(warningImage_, WARNING_IMAGE_RECT);

	Vector2F screenSize = Game::System<Screen>().GetSizeF();
	// 画面上の敵を検出する距離
	float detectDistance = Game::System<CameraSystem>().GetFar();

	// ターゲットの強調表示の画像、画像サイズ
	highlightFrameImage_ = Image::Load("Image/highlightEnemyFrame.png");
	enemyArrowImage_ = Image::Load("Image/enemyArrow.png");
	highlightFrameSize_ = { 60.0f,60.0f };
	enemyArrowImageSize_ = { 30.0f,30.0f };
	
	// 敵の射撃時に呼ばれるコールバック
	projectionEventHandlerId_ = Game::System<EventManager>().GetEvent<ProjectTile::EventData>().Subscribe(
		[this](const ProjectTile::EventData& _data)
		{
			ProjectionEventHandler(_data);
		});

	enemyAIEventHandlerId_ = Game::System<EventManager>().GetEvent<EnemyAI::EventData>().Subscribe(
		[this](const EnemyAI::EventData& _data)
		{
			OnEnemyAIStateChanged(_data);
		});

	// 字幕
	pTextBox_ = Instantiate<TextBox>(0.01f, GenDrawScreenFrom(SPEECH_TEXT_POS), GenDrawScreenFontSize(SPEECH_TEXT_SIZE));

	speechQueueMap_.emplace(SpeechType::FireMissle, ShuffleSpeechQueue{ speechLinesOnFiredMissile });
}

ControlTower::~ControlTower()
{
	Game::System<EventManager>().GetEvent<ProjectTile::EventData>().Unsubscribe(projectionEventHandlerId_);
	Game::System<EventManager>().GetEvent<EnemyAI::EventData>().Unsubscribe(enemyAIEventHandlerId_);
	delete pWarningBlinker_;
}

void ControlTower::Update()
{
	detectedEnemyIds_.erase(std::remove(detectedEnemyIds_.begin(), detectedEnemyIds_.end(), INVALD_ENTITY), detectedEnemyIds_.end());

	for (auto& detector : wndRectDetector_)
	{
		detector.second.UpdateDetection();
	}
	
}

void ControlTower::Draw() const
{
	DrawEnemies(detectedEnemyIds_);
	DrawEnemies(detectedMissileIds_);

	if (currentThreatLevel_ == ThreatLevel::Danger)
	{
		pWarningBlinker_->Draw();
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
			.base = 
			{
				.targetTag = GameObjectTag::Enemy,
				.windowContext = _context,
				.maxDistance = detectDistance,
			},
			.detectionRect =
			{
				0.0f,
				0.0f,
				static_cast<float>(screenSize.x),
				static_cast<float>(screenSize.y),
			},
		};
		
		wndRectDetector_.try_emplace(_context, config);
	}
}

void ControlTower::ProjectionEventHandler(const ProjectTile::EventData& _data)
{
	switch (_data.eventType)
	{
	case ProjectTile::EventType::Fired:
		OnProjectionFired(_data);
		break;
	case ProjectTile::EventType::Hit:
		OnProjectionHit(_data);
		break;
	case ProjectTile::EventType::Destroyed:
		OnProjectionDestroyed(_data);
		break;
	}
}

void ControlTower::OnProjectionFired(const ProjectTile::EventData& _data)
{
	if (_data.type == ProjectTile::Type::Missile && _data.shooter == ProjectTile::Shooter::Enemy)
	{
		auto itr = std::find(detectedMissileIds_.begin(), detectedMissileIds_.end(), _data.id);
		// IDが重複していないなら追加
		if (itr != detectedMissileIds_.end()) return;
		detectedMissileIds_.push_back(_data.id);

		// 警告のテキスト、音声
		SPEECH_ELEMENT speechElement;
		auto speechItr = speechQueueMap_.find(SpeechType::FireMissle);
		if (speechItr != speechQueueMap_.end())
		{
			if (speechItr->second.TryGetNext(speechElement))
			{
				Speech(speechElement);
			}
		}

		// 警告の画像表示
		pWarningBlinker_->StartBlink(BLINK_INTERVAL);

		// 脅威度の変化を通知
		currentThreatLevel_ = ThreatLevel::Danger;
		Game::System<EventManager>().GetEvent<ThreatEventData>().Invoke(
			{
				.level = ThreatLevel::Danger
			});
	}
}

void ControlTower::OnProjectionHit(const ProjectTile::EventData& _data)
{

}

void ControlTower::OnProjectionDestroyed(const ProjectTile::EventData& _data)
{
	if (_data.type == ProjectTile::Type::Missile && _data.shooter == ProjectTile::Shooter::Enemy)
	{
		// ミサイルをリストから削除
		detectedMissileIds_.erase(
			std::remove(detectedMissileIds_.begin(), detectedMissileIds_.end(), _data.id),
			detectedMissileIds_.end());

		// 他にミサイルがなければ通常状態に戻す
		if (detectedMissileIds_.empty())
		{
			LOGIMGUI_CAT("ControlTower", "ミサイルが破棄された");
			pWarningBlinker_->StopBlink();
			currentThreatLevel_ = ThreatLevel::Normal;
			Game::System<EventManager>().GetEvent<ThreatEventData>().Invoke(
				{
					.level = ThreatLevel::Normal
				});
		}
	}
}

void ControlTower::OnEnemyAIStateChanged(const EnemyAI::EventData& _data)
{
	if (_data.mainState == EnemyAI::MAIN_STATE::S_FIGHT)
	{
		auto itr = std::find(detectedEnemyIds_.begin(), detectedEnemyIds_.end(), _data.id);
		// IDが重複していないなら追加
		if (itr == detectedEnemyIds_.end())
		{
			detectedEnemyIds_.push_back(_data.id);
		}
	}
	else
	{
		detectedEnemyIds_.erase(std::remove(detectedEnemyIds_.begin(), detectedEnemyIds_.end(), _data.id), detectedEnemyIds_.end());
	}
}

void ControlTower::Speech(const SPEECH_ELEMENT& _speechElement)
{
	LOGIMGUI_CAT("ControlTower", "Speech");
	pTextBox_->Show(_speechElement.text_.data());
	Game::System<Audio>().PlayOneShotFile(_speechElement.audioFile_.data());

	Timer& timer = Game::System<Timer>();
	timer.Remove(hTimer_);
	hTimer_ = timer.AddAram(_speechElement.time_, [this]()
		{
			pTextBox_->Hide();
		});
}

void ControlTower::DetectionEnemy(Transform* _transform)
{
	detectedEnemyIds_.clear();

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
				if (currState != EnemyAI::MAIN_STATE::S_FIGHT || _enemy->IsToDestroy())
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

	detectedEnemyIds_.push_back(enemyId);
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

void ControlTower::DrawEnemies(const std::vector<EntityId>& _ids) const
{
	if (auto itr = wndRectDetector_.find(CurrContext()); itr != wndRectDetector_.end())
	{
		const RectDetector& detector = itr->second;

		std::vector<EntityId> outOfScreenTargets;
		// 攻撃状態の敵
		for (EntityId enemy : _ids)
		{
			// 画面上に検出した敵がいるか確認
			auto& detectedTargets = detector.GetDetectedTargets();
			bool isOnScreen = std::any_of(detectedTargets.begin(), detectedTargets.end(),
				[enemy](const ScreenCoordContainsInfo& _info)
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
