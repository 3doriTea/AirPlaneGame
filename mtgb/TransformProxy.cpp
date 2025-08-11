#include "TransformProxy.h"


TransformProxy::TransformProxy(Transform* transform)
	:ImGuiShowable<Transform>(transform,Show::Inspector)
	,position_{&target_->position_}
	,rotate_{&target_->rotate_}
	,scale_{&target_->scale_}
{
	isAuto_ = true;
}



