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
	auto& collection = collectionMap_[CurrContext()];

	for (auto& resource : collection)
	{
		resource.second->Update();
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

void mtgb::WindowContextResourceManager::OnResizeAll(WindowContext windowContext, UINT width, UINT height)
{
	auto itr = collectionMap_.find(windowContext);
	if (itr == collectionMap_.end()) return;

	collectionMap_[windowContext].ForEachInReverseOrder(
		[](const std::type_index&, WindowContextResource* resource)
		{
			if (resource)
			{
				resource->Reset();
			}
		}
	);

	collectionMap_[windowContext].ForEachInOrder(
		[windowContext, width, height](const std::type_index&, WindowContextResource* resource)
		{
			if (resource)
			{
				resource->OnResize(windowContext, width, height);
			}
		}
	);



}




