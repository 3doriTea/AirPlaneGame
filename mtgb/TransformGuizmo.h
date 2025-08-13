#pragma once
#include "ImGuiShowable.h"
#include "Transform.h"

typedef unsigned int        ImGuiID;
class TransformGuizmo : public ImGuiShowable<Transform>
{
public:
	TransformGuizmo(Transform* transform);
	void ShowImGui() override;
private:
	ImGuiID id;
};
