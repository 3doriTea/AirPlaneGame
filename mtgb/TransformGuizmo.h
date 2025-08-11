#pragma once
#include "ImGuiShowable.h"
#include "mtgb.h"

class TransformGuizmo : public ImGuiShowable<Transform>
{
public:
	TransformGuizmo(Transform* transform);
	void ShowImGui() override;
};
