#include "RenderResourceManager.h"

mtgb::RenderResourceManager::RenderResourceManager()
{
	pCollection_ = nullptr;
}

void mtgb::RenderResourceManager::Initialize()
{
}

void mtgb::RenderResourceManager::Update()
{
}

void mtgb::RenderResourceManager::Release()
{
	if (pCollection_ != nullptr)
	{
		delete pCollection_;
	}
}

void mtgb::RenderResourceManager::CreateRenderResource(HWND hWnd)
{
	pCollection_->CreateResource(hWnd);
}

void mtgb::RenderResourceManager::ChangeRenderResource(HWND hWnd)
{
	pCollection_->ChangeResource(hWnd);
}
