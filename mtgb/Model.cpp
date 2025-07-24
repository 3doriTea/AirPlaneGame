#include "Model.h"
#include "Fbx.h"
#include "PathUtility.h"
#include "MTAssert.h"
#include "mtgb.h"


mtgb::Model::Model()
{
}

mtgb::Model::~Model()
{
}

void mtgb::Model::Initialize()
{
}

void mtgb::Model::Update()
{
}

mtgb::ModelHandle mtgb::Model::Load(const std::string_view& _fileName)
{
	Model& instance{ Game::System<Model>() };

	// ファイルの拡張子
	std::string_view fileExt{ FilePath::GetExtension(_fileName) };

	/*
	Sprite* pSprite{ new Sprite{} };
	pSprite->Initialize();
	pSprite->Load(ToWString(_fileName));
	ImageHandle handle{ ++instance.handleCounter_ };
	instance.sprites_.insert({ handle , pSprite });

	return handle;
	*/

	ModelData* pModelData{ new ModelData{} };

	if (fileExt == "fbx")
	{
		instance.modelData_.insert({  } );
	}
	else if (fileExt == "obj")
	{
		massert(false && "objファイルは未実装です。");
	}
	else
	{
		// fbx, obj でもないなら
		massert(false && "非対応のモデルファイルです。");
	}
	return INVALID_HANDLE;
}
