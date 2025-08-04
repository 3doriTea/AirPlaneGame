#pragma once
#include <mtgb.h>

class Camera : public GameObject
{
public:
	Camera(const Vector3& _position, mtgb::WindowContext _context);
	~Camera();

	void Update();
	void Draw() const;

private:
	Transform* pTransform_;
	WindowContext context_;
};
