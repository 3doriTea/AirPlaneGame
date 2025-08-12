#include "TransformGuizmo.h"
#include "Draw.h"

TransformGuizmo::TransformGuizmo(Transform* transform)
	:ImGuiShowable<Transform>(transform,Show::GameView)
{
	isAuto_ = true;
}

void TransformGuizmo::ShowImGui()
{
	Draw::TransformGuizmo(target_);
}
