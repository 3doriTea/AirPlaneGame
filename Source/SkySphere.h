#pragma once
#include <mtgb.h>
#include "AccessibleMacro.h"
#include "Camera.h"

//REFL_FORWARD_DECLARATION
REFL_FORWARD_DECLARATION()
class SkySphere : public GameObject
{
	ACCESSIBLE_PRIVATE()
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
