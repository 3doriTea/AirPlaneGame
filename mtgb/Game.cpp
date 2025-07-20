#include "Game.h"
#include "ComponentPool.h"
//#include "Windows.h"
#include "IComponentPool.h"


mtgb::Game::Game() :
	pRegisterSystems_{},
	pFrameUpdateSystems_{},
	pFixedUpdateSystems_{},
	pComponentPools_{}
{
}

mtgb::Game::~Game()
{
}

void mtgb::Game::Exit()
{
	toExit_ = true;
}

void mtgb::Game::UpdateFrame()
{
	for (auto&& updateSystem : pInstance_->pFrameUpdateSystems_)
	{
		updateSystem->Update();
	}
}

void mtgb::Game::UpdateFixed()
{
	for (auto&& updateSystem : pInstance_->pFixedUpdateSystems_)
	{
		updateSystem->Update();
	}
}

void mtgb::Game::RemoveEntityComponent(const EntityId _entityId)
{
	for (auto&& cpSystem : pInstance_->pComponentPools_)
	{
		cpSystem->Remove(_entityId);
	}
}

void mtgb::Game::InitializeSystems(const std::list<ISystem*>& _uninitialized)
{
	for (auto&& pRegisterSystem : _uninitialized)
	{
		pRegisterSystem->
			Initialize();
	}
}

void mtgb::Game::ReleaseSystems(const std::list<ISystem*>& _runnings)
{
	for (auto&& pRegisterSystem : _runnings)
	{
		delete pRegisterSystem;
	}
}

void mtgb::Game::RunLoopGameCycle()
{
	while (true)
	{
		for (auto&& updateSystem : pInstance_->pCycleUpdateSystems_)
		{
			updateSystem->Update();
		}

		if (toExit_)  // 終了フラグが立っていたらサイクル離脱
		{
			break;
		}
	}
}

mtgb::Game* mtgb::Game::pInstance_{ nullptr };
bool mtgb::Game::toExit_{ false };
