#include "PlayerProxy.h"
#include "Player.h"
#include "TransformProxy.h"

PlayerProxy::PlayerProxy(Player* player)
	:ImGuiShowable<Player>(player)
	,test1(&target_->test1)
	,test2(&target_->test2)
	,name(target_->name_)

{
	pTransformProxy_ = new TransformProxy(target_->pTransform_);
	isAuto_ = true;
}
