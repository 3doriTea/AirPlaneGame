#pragma once
#include <mtgb.h>
#include "Camera.h"

class SkySphere : public GameObject
{
public:
	SkySphere();
	SkySphere(mtgb::WindowContext context);
	~SkySphere();

	void Update() override;
	void Draw() const override;

private:
	Transform*          pTransform_;
	mtgb::WindowContext context_;
	FBXModelHandle      hModel_;
};
