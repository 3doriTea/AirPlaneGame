#pragma once
#include <mtgb.h>

class Camera : public GameObject
{
public:
	Camera();
	~Camera();

	void Update();
	void Draw() const;

private:
	Transform* pTransform_;
};
