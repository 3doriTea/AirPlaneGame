#include "PlayerProxy.h"
#include "Player.h"
#include "TransformProxy.h"
#include "TransformGuizmo.h"

PlayerProxy::PlayerProxy(Player* player)
	:ImGuiShowable<Player>(player,Show::Inspector)
	,test1(&target_->test1)
	,test2(&target_->test2)
	,name(target_->name_)

{
	pTransformProxy_ = new TransformProxy(target_->pTransform_);
	pTransformGuizmo_ = new TransformGuizmo(target_->pTransform_);
	isAuto_ = false;
}

void PlayerProxy::ShowImGui()
{
	Inspector::Instance().ShowInspector(pTransformProxy_, "Transform");

	if (CurrContext() == WindowContext::First)
	{
		Draw::TransformGuizmo(target_->pTransform_);
	}
}
