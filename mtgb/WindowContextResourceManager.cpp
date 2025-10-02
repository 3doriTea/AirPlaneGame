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

void mtgb::WindowContextResourceManager::ChangeActiveResource(WindowContext windowContext)
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

//void mtgb::WindowContextResourceManager::SwapAllResource(WindowContext context1, WindowContext context2)
//{
//	// 念のため同じウィンドウを指定していないか確認
//	if (context1 == context2)
//	{
//		return;
//	}
//
//	// context1,2のResourceCollectionが登録されている確認
//	auto itr1 = collectionMap_.find(context1);
//	auto itr2 = collectionMap_.find(context2);
//
//	assert(itr1 != collectionMap_.end() && "指定されたWindowContextが見つかりません");
//	assert(itr2 != collectionMap_.end() && "指定されたWindowContextが見つかりません");
//
//	/*itr1->second.ForEachInOrder(
//		[&](const std::type_index& key, WindowContextResource* resource)
//		{
//			std::swap(itr1->second[key], itr2->second[key]);
//		}
//	);*/
//
//	// ResourceCollection全体をswap
//	itr1->second.Swap(itr2->second);
//}




