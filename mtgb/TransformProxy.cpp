#include "TransformProxy.h"

TransformProxy::TransformProxy(Transform* transform)
	:ImGuiShowable<Transform>(transform)
	,position_{&target_->position_}
	,rotate_{&target_->rotate_}
	,scale_{&target_->scale_}
{
	isAuto_ = true;
}
