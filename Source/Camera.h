#pragma once
#include <mtgb.h>

class Camera : public GameObject
{
public:
	Camera(const Vector3& _position);
	~Camera();

	void Update();
	void Draw() const;

private:
	Transform* pTransform_;
};
