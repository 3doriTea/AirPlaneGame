#include "Fbx.h"
#include "ReleaseUtility.h"
#include "FbxParts.h"
#include "DirectX11Draw.h"
#include "MTAssert.h"
#include "MTAssert.h"


mtgb::Fbx::Fbx() :
	pFbxManager_{ nullptr }
{
}

mtgb::Fbx::~Fbx()
{
	Release();
}

void mtgb::Fbx::Initialize()
{
	pFbxManager_ = FbxManager::Create();
}

void mtgb::Fbx::Update()
{
}

mtgb::FBXModelHandle mtgb::Fbx::Load(const std::string& _fileName)
{
	Fbx& instance{ Game::System<Fbx>() };

	FbxModel* pFbxModel{ new FbxModel{} };

	pFbxModel->Load(_fileName);
	FBXModelHandle handle{ ++instance.handleCounter_ };
	instance.pFbxModels_.insert({ handle, pFbxModel });

	
	return handle;
}

void mtgb::Fbx::Draw(const FBXModelHandle _hModel, const Transform& _transfrom, int _frame)
{
	DirectX11Draw::SetBlendMode(BlendMode::Default);

	massert((0 < _hModel) && (_hModel <= handleCounter_)
		&& "–³Œø‚Èƒnƒ“ƒhƒ‰ @Fbx::Draw");

	massert(pFbxModels_.count(_hModel) != 0
		&& "ƒ‚ƒfƒ‹‚ª‘¶Ý‚µ‚È‚¢ @Fbx::Draw");

	pFbxModels_[_hModel]->Draw(_transfrom, _frame);
}

void mtgb::Fbx::Release()
{
	for (auto& pFbxModel : pFbxModels_)
	{
		pFbxModel.second->Release();
	}
	pFbxModels_.clear();
	SAFE_DESTROY(pFbxManager_);
}


