#pragma once
#include <mtgb.h>

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();

	void Update() override;
	void Draw() const override;
private:
	int hp_;
	float speed_;
	Transform* pTransform_;
	//OBJModelHandle hModel_;
	FBXModelHandle hModel_;
	float radius_;
};
