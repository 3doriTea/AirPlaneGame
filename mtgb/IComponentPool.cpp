#include "IComponentPool.h"
#include "SceneSystem.h"

mtgb::IComponentPool::IComponentPool()
{
}

mtgb::IComponentPool::~IComponentPool()
{
}

void mtgb::IComponentPool::RegisterCurrentScene(const std::function<void()>& _onMove)
{
	Game::System<SceneSystem>().OnMove(_onMove);
}
