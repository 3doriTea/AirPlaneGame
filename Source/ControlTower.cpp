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
			.time_ = 3.5f
		},
		{
			.text_ = u8"ミサイル接近!撃ち落として!",
			.audioFile_ = "Sound/Voice/016_ずんだもん（ノーマル）_ミサイル接近!撃ち….wav",
			.time_ = 4.5f
		},
	};
	SpeechLines speechLinesOnHit =
	{
		{
			.text_ = u8"被弾したよ!",
			.audioFile_ = "Sound/Voice/013_ずんだもん（ノーマル）_被弾したよ!.wav",
			.time_ = 2.5f
		},
		{
			.text_ = u8"ぬわーーーーーっ!",
			.audioFile_ = "Sound/Voice/014_ずんだもん（ノーマル）_ぬわわわわわわわ.wav",
			.time_ = 3.0f
		},
	};


	




	// 字幕の表示位置
	const Vector2F SPEECH_TEXT_POS{ 620, 820 };
	// 字幕のフォントサイズ
	const int SPEECH_TEXT_SIZE{ 48 };

	const RectF WARNING_IMAGE_RECT{ 830,170,260,60 };
	

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
	enemyArrowImage_ = Image::Load("Image/RedArrow.png");
	missileArrowImage_ = Image::Load("Image/BlackArrow.png");

	highlightFrameSize_ = { 60.0f,60.0f };
	targetArrowImageSize_ = { 30.0f,30.0f };
	
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
	speechQueueMap_.emplace(SpeechType::Hit, ShuffleSpeechQueue{ speechLinesOnHit });
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
	
	DrawEnemies(detectedEnemyIds_, enemyArrowImage_);
	DrawEnemies(detectedMissileIds_, missileArrowImage_);

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
		// IDが重複しているなら回帰
		if (itr != detectedMissileIds_.end()) return;
		detectedMissileIds_.push_back(_data.id);

		// 脅威度が既にDangerなら回帰
		if (currentThreatLevel_ == ThreatLevel::Danger) return;
		// 警告のテキスト、音声
		Speech(SpeechType::FireMissle);

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
	Speech(SpeechType::Hit);
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

void ControlTower::Speech(SpeechType _speechType)
{
	SPEECH_ELEMENT speechElement;
	auto speechItr = speechQueueMap_.find(_speechType);
	if (speechItr == speechQueueMap_.end()) return;
	
	if (speechItr->second.TryGetNext(speechElement) == false) return;
		
	pTextBox_->Show(speechElement.text_.data());
	Game::System<Audio>().PlayOneShotFile(speechElement.audioFile_.data());

	Timer& timer = Game::System<Timer>();
	timer.Remove(hTimer_);
	hTimer_ = timer.AddAram(speechElement.time_, [this]()
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
}

void ControlTower::DrawEnemies(const std::vector<EntityId>& _ids, ImageHandle _image) const
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
			EnemyArrowInfo info = ComputeEnemyArrowInfo(outOfScreenTarget);

			if (info.invalid) continue;
			DrawArrowAtPosition(info.position, info.angle, _image);
		}
	}
}

ControlTower::EnemyArrowInfo ControlTower::ComputeEnemyArrowInfo(EntityId _entityId) const
{
	// 現在のカメラ(プレイヤー)の Transformを取得
	EnemyArrowInfo info{};
	info.invalid = true;

	WindowContext context = CurrContext();
	if (controlTargetTransform_.contains(context) == false) return info;

	Transform* pCameraTransform = controlTargetTransform_.find(context)->second;
	if (!pCameraTransform) return info;

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
	info.position = arrowPos;
	info.angle = -angle;
	info.invalid = false;
	return info;
}

void ControlTower::DrawArrowAtPosition(const Vector2F& _position, float _angle, ImageHandle _image) const
{
	// 矢印の画像を回転させて描画
	RectF drawRect =
	{
		_position.x - (targetArrowImageSize_.x * 0.5f),
		_position.y - (targetArrowImageSize_.y * 0.5f),
		targetArrowImageSize_.x,
		targetArrowImageSize_.y
	};

	Draw::Image(_image, drawRect, { Vector2F::Zero(),Image::GetSizeF(_image) }, _angle);
}
