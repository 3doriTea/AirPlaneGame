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
	// �t�@�C���̊g���q
	std::string_view fileExt{ FilePath::GetExtension(_fileName) };

	if (fileExt == "fbx")
	{
		//modelDatas_.insert(new ModelData{});
	}
	else if (fileExt == "obj")
	{
		massert(false && "obj�t�@�C���͖������ł��B");
	}
	else
	{
		// fbx, obj �ł��Ȃ��Ȃ�
		massert(false && "��Ή��̃��f���t�@�C���ł��B");
	}
	return INVALID_HANDLE;
}
