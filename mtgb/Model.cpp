#include "Model.h"
#include "Fbx.h"
#include "PathUtility.h"
#include "MTAssert.h"


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
	// ファイルの拡張子
	std::string_view fileExt{ FilePath::GetExtension(_fileName) };

	if (fileExt == "fbx")
	{
		//modelDatas_.insert(new ModelData{});
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
