#pragma once
#include <mtgb.h>
#include "PlayScene/EnemyAI.h"
#include "PlayScene/Gun.h"


class EnemiesController;

class Enemy : public GameObject
{
public:
	Enemy(const Vector3& _position, const EntityId _controllerId);
	~Enemy();

	void Update() override;
	void Draw() const override;
private:
	int hp_;
	float speed_;
	Transform* pTransform_;
	//OBJModelHandle hModel_;
	EntityId controllerId_;

	FBXModelHandle hModel_;
	FBXModelHandle hModelCollider_;
	float radius_;

	EnemyAI ai_;  // ai

	Gun gun_;  // 機銃管理の部品クラス

	RigidBody* pRigidBody_;
	EnemiesController* pEnemiesController_;
};
