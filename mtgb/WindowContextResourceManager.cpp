#include "WindowContextResourceManager.h"

mtgb::WindowContextResourceManager::WindowContextResourceManager()
{
	pCollection_ = nullptr;
}

void mtgb::WindowContextResourceManager::Initialize()
{
}

void mtgb::WindowContextResourceManager::Update()
{
}

void mtgb::WindowContextResourceManager::Release()
{
	if (pCollection_ != nullptr)
	{
		delete pCollection_;
	}
}

void mtgb::WindowContextResourceManager::CreateResource(HWND hWnd)
{
	pCollection_->CreateResource(hWnd);
}

void mtgb::WindowContextResourceManager::ChangeResource(HWND hWnd)
{
	pCollection_->ChangeResource(hWnd);
}
