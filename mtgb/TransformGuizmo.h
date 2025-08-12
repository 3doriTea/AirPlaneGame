#pragma once
#include "ImGuiShowable.h"
#include "Transform.h"

class TransformGuizmo : public ImGuiShowable<Transform>
{
public:
	TransformGuizmo(Transform* transform);
	void ShowImGui() override;
};
