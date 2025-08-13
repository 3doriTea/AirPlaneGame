#include "WindowContextResourceManager.h"


mtgb::WindowContextResourceManager::WindowContextResourceManager()
{
}

mtgb::WindowContextResourceManager::~WindowContextResourceManager()
{
	Release();
}

void mtgb::WindowContextResourceManager::Initialize()
{
}

void mtgb::WindowContextResourceManager::Update()
{
	for (auto& collection : collectionMap_)
	{
		for (auto& resource : collection.second)
		{
			resource.second->Update();
		}
	}
}

void mtgb::WindowContextResourceManager::Release()
{
	collectionMap_.clear();
}

void mtgb::WindowContextResourceManager::CreateResource(WindowContext windowContext)
{
	collectionMap_[windowContext] = defResource_;
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

