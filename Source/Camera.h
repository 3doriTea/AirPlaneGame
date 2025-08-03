#pragma once
#include <mtgb.h>

class Camera : public GameObject
{
public:
	Camera();
	Camera(mtgb::WindowContext context);
	~Camera();

	void Update();
	void Draw() const;

private:
	Transform* pTransform_;
	WindowContext context_;
};
