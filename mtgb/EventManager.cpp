#include "EventManager.h"
#include "Game.h"
#include "SceneSystem.h"
void mtgb::EventManager::Initialize()
{
	Game::System<SceneSystem>().RegisterPendingCallback([this]()
		{
			for (auto& event : events_)
			{
				event.second->UnsubscribeAll();
			}
		});
}