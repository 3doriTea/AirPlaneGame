#pragma once
#include <mtgb.h>
#include <vector>
#include <tuple>
#include <optional>
#include <map>
class ControlTower : public mtgb::GameObject
{
public:
	ControlTower();
	~ControlTower();

	void Update() override;
	void Draw() const override;
	void SetControlTarget(EntityId _id, WindowContext _context);
private:
	std::pair<Transform*, WindowContext> pGunner_;
	std::pair<Transform*, WindowContext> pPilot_;
	Transform* pPlayerPlaneTransform_;

	float detectionRadius_;// プレイヤーを原点にして検出をする球の半径
	std::vector<EntityId> attackStateEnemies_;// 検出した敵のID

	std::map<WindowContext, Transform*> controlTargetTransform_;
	std::map<WindowContext, RectDetector> wndRectDetector_;
	ImageHandle highlightFrameImage_; // ターゲット強調表示の画像
	ImageHandle enemyArrowImage_; // 画面外のターゲットの方角を示す画像
	Vector2F highlightFrameSize_; // ターゲット強調表示の画像のサイズ
	Vector2F enemyArrowImageSize_; // 画面外のターゲットの方角を表す画像のサイズ
	void DetectionEnemy(Transform* _transform);


	// 画面外の敵の方角を矢印で表示する
	void DrawEnemyArrow(EntityId _entityId) const;

	void DrawArrowAtPosition(const Vector2F& _position, float _angle) const;
};
