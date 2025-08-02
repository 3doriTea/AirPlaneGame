#include "WindowContextResourceManager.h"
mtgb::WindowContext mtgb::WindowContextResourceManager::currentContext_{ WindowContext::None };

mtgb::WindowContextResourceManager::WindowContextResourceManager()
{
}

void mtgb::WindowContextResourceManager::Initialize()
{
}

void mtgb::WindowContextResourceManager::Update()
{
}

void mtgb::WindowContextResourceManager::Release()
{

}

void mtgb::WindowContextResourceManager::CreateResource(WindowContext windowContext)
{
	collectionMap_[windowContext] = defResource_.Clone();
	collectionMap_[windowContext].ForEachInOrder(
		[windowContext](const std::type_index&,WindowContextResource* resource)
		{
			if (resource)
			{
				resource->Initialize(windowContext);
			}
		}
	);
}

void mtgb::WindowContextResourceManager::ChangeResource(WindowContext windowContext)
{
	auto itr = collectionMap_.find(windowContext);
	assert(itr != collectionMap_.end() && "指定されたWindowContextのリソースが見つかりません");

	for (auto& collection : itr->second)
	{
		collection.second->SetResource();
	}

	currentContext_ = windowContext;
}

HWND mtgb::WindowContextResourceManager::GetHWND(WindowContext windowContext)
{
	return collectionMap_[windowContext].Get<WindowResource>().hWnd_;
}
