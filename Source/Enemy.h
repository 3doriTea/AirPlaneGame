#pragma once
#include <mtgb.h>

class Enemy : public GameObject
{
public:
	Enemy(const Vector3& _position);
	~Enemy();

	void Update() override;
	void Draw() const override;
	void Search();
private:
	int hp_;
	float speed_;
	Transform* pTransform_;
	static Transform* pPlayerPlaneTransform_;
	//OBJModelHandle hModel_;
	FBXModelHandle hModel_;
	FBXModelHandle hModelCollider_;
	float radius_;
	float lockOnAngle_;// ロックオン、プレイヤーを発見する角度(degree)
	float lockOnDistance_;// ロックオン、プレイヤーを発見する距離
};
