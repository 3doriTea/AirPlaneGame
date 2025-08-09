#include "PlayerProxy.h"
#include "Player.h"

PlayerProxy::PlayerProxy(Player* player)
	:ImGuiShowable<Player>(player)
	,test1(&target_->test1)
	,test2(&target_->test2)
	,name(target_->name_)
{
	isAuto_ = false;
}
