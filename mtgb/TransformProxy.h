#pragma once
#include "ImGuiShowable.h"
#include "mtgb.h"

class TransformProxy : public ImGuiShowable<Transform>
{
public:
	TransformProxy(Transform* transform);
	void ShowImGui() override;
	Vector3* position_;
	Quaternion* rotate_;
	Vector3* scale_;
};