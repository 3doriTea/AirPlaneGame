#pragma once
#include <mtgb.h>
#include "../Health.h"
#include "../PlayScene/EnemyAI.h"
#include "../PlayScene/Gun.h"
#include "../TargetingSystem.h"
class EnemiesController;


class EnemyPlane : public GameObject
{
public:
	EnemyPlane(const Vector3& _worldPosition, const EntityId _playerPlane, const EntityId _controllerId);
	~EnemyPlane();

	void Update() override;
	void Draw() const override;
	void Search();
	void Fight(const EnemyAI::OutData& _outData); // 戦闘行動
	bool HandleCrash(); // 撃墜された際の処理
	const EnemyAI& GetAI() const { return ai_; }
	bool IsActive() const;
private:
	FBXModelHandle hModel_;
	RigidBody* pRB_;
	Collider* pCollider_;
	Transform* pTarget_;
	Transform* pTransform_;

	EnemyAI ai_;  // ai
	Gun gun_;     // 機銃管理の部品クラス

	EnemiesController* pEnemiesController_;
	EntityId controllerId_;
	TargetingSystem targetingSystem_;

	Health health_;   // 体力
	float speed_;  // 移動速度 m/s
	bool broken_;  // 壊れているか
	float lockOnAngle_;// ロックオン、プレイヤーを発見する角度(degree)
	float lockOnDistance_;// ロックオン、プレイヤーを発見する距離
	float timeSinceLastshot_;
};
