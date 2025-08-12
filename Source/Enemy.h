#pragma once
#include <mtgb.h>

class Enemy : public GameObject
{
public:
	Enemy(const Vector3& _position);
	~Enemy();

	void Update() override;
	void Draw() const override;
private:
	int hp_;
	float speed_;
	Transform* pTransform_;
	//OBJModelHandle hModel_;
	FBXModelHandle hModel_;
	FBXModelHandle hModelCollider_;
	float radius_;
};
