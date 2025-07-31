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

void mtgb::WindowContextResourceManager::CreateResource(WindowContext windowContext)
{
	pCollection_->CreateResource(windowContext);
}

void mtgb::WindowContextResourceManager::ChangeResource(WindowContext windowContext)
{
	pCollection_->ChangeResource(windowContext);
}

HWND mtgb::WindowContextResourceManager::GetHWND(WindowContext windowContext)
{
	return pCollection_->GetHWND(windowContext);
}
