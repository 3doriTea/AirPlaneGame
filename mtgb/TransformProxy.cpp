#include "TransformProxy.h"


TransformProxy::TransformProxy(Transform* transform)
	:ImGuiShowable<Transform>(transform,Show::Inspector)
	,position_{&target_->position}
	,rotate_{&target_->rotate}
	,scale_{&target_->scale}
{
	isAuto_ = true;
}

void TransformProxy::ShowImGui()
{
	Inspector::Instance().ShowInspector(this, displayName_.c_str());
}



