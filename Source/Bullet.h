#pragma once
#include <mtgb.h>

class Bullet : public mtgb::GameObject
{
public:
	Bullet(const Vector3& _position);
	~Bullet();

	void Update() override;
	void Draw() const override;

private:
	ImageHandle hImage_;
	Transform* pTransform_;
	RigidBody* pRb_;
};
