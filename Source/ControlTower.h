#pragma once
#include <mtgb.h>
#include <vector>
#include <tuple>
#include <optional>
#include <map>
#include "ProjectTile.h"
#include "../Source/TextBox.h"
#include "TutorialScene/SpeechQueue.h"
#include "../Source/PlayScene/EnemyAI.h"
#include "../Source/ImageBlinker.h"
#include "../Source/ThreatData.h"
#include "ShuffleSpeechQueue.h"

class ControlTower : public mtgb::GameObject
{
	enum class SpeechType
	{
		FireMissle,
		Hit,
	};
public:
	ControlTower();
	~ControlTower();

	void Update() override;
	void Draw() const override;
	void SetControlTarget(EntityId _id, WindowContext _context);
private:
	Transform* pPlayerPlaneTransform_;

	float detectionRadius_;// プレイヤーを原点にして検出をする球の半径
	std::vector<EntityId> detectedEnemyIds_;// 検出した敵のID
	std::vector<EntityId> detectedMissileIds_; // 検出したミサイルのID

	std::map<WindowContext, Transform*> controlTargetTransform_;
	std::map<WindowContext, RectDetector> wndRectDetector_;
	ImageHandle warningImage_; // 警告の画像
	ImageHandle highlightFrameImage_; // ターゲット強調表示の画像
	ImageHandle enemyArrowImage_; // 画面外のターゲットの方角を示す画像
	Vector2F highlightFrameSize_; // ターゲット強調表示の画像のサイズ
	Vector2F enemyArrowImageSize_; // 画面外のターゲットの方角を表す画像のサイズ

	TextBox* pTextBox_; // 字幕
	TimerHandle hTimer_;
	EventHandlerId projectionEventHandlerId_;
	EventHandlerId enemyAIEventHandlerId_;
	ImageBlinker* pWarningBlinker_;
	ThreatLevel currentThreatLevel_;

	std::map<SpeechType,ShuffleSpeechQueue> speechQueueMap_;
private:
	void ProjectionEventHandler(const ProjectTile::EventData& _data);
	void OnProjectionFired(const ProjectTile::EventData& _data);
	void OnProjectionHit(const ProjectTile::EventData& _data);
	void OnProjectionDestroyed(const ProjectTile::EventData& _data);
	void OnEnemyAIStateChanged(const EnemyAI::EventData& _data);

	void Speech(const SPEECH_ELEMENT& _speechElement);
	void DetectionEnemy(Transform* _transform);

	void DrawEnemies(const std::vector<EntityId>& _ids) const;
	// 画面外の敵の方角を矢印で表示する
	void DrawEnemyArrow(EntityId _entityId) const;

	void DrawArrowAtPosition(const Vector2F& _position, float _angle) const;

	
};
