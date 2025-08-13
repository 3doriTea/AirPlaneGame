#include "TransformGuizmo.h"
#include "Draw.h"

namespace
{
	static ImGuiID idCount = 0;
}

TransformGuizmo::TransformGuizmo(Transform* transform)
	:ImGuiShowable<Transform>(transform,Show::GameView)
{
	isAuto_ = true;
	id = idCount++;
}

void TransformGuizmo::ShowImGui()
{
	Draw::TransformGuizmo(target_);
}
