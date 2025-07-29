#include "Fbx.h"
#include "ReleaseUtility.h"
#include "FbxParts.h"
#include "DirectX11Draw.h"
#include "MTAssert.h"


mtgb::Fbx::Fbx()
{
}

mtgb::Fbx::~Fbx()
{
	Release();
}

void mtgb::Fbx::Load(const std::string& _fileName)
{
	pFbxManager_ = FbxManager::Create();
	pFbxScene_ = FbxScene::Create(pFbxManager_, "fbxscene");
	FbxString fileName{ _fileName.c_str() };
	FbxImporter* fbxImporter{ FbxImporter::Create(pFbxManager_, "imp") };

	massert(fbxImporter->Initialize(fileName.Buffer(), -1, pFbxManager_->GetIOSettings())
		&& "fbxImporter�̏������Ɏ��s���� @Fbx::Load");

	fbxImporter->Import(pFbxScene_);
	SAFE_DESTROY(fbxImporter);

	FbxGeometryConverter geometryConverter{ pFbxManager_ };

	// �A�j���[�V�����^�C�����[�h�̎擾
	frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

	// ���݂̃J�����g�f�B���N�g�����擾
	char defaultCurrentDirectory[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	// �J�����g�f�B���N�g�����ړ�
	char directory[MAX_PATH]{};
	_splitpath_s(_fileName.c_str(), nullptr, 0, directory, MAX_PATH, nullptr, 0, nullptr, 0);
	SetCurrentDirectory(directory);

	int meshCount{ pFbxScene_->GetSrcObjectCount<FbxMesh>() };
	for (int i = 0; i < meshCount; i++)
	{
		// ���ׂẴ��b�V���f�[�^���擾�ł���炵��
		FbxMesh* pMesh{ pFbxScene_->GetSrcObject<FbxMesh>() };

		FbxParts* pParts{ new FbxParts{} };
	}
}

void mtgb::Fbx::Draw(const Transform& _transfrom, int _frame)
{
	DirectX11Draw::SetBlendMode(BlendMode::Default);

	for (int i = 0; i < pParts_.size(); i++)
	{
		// �A�j���[�V�����^�C���̎p���s����擾����
		FbxTime time;
		time.SetTime(0, 0, 0, _frame, 0, 0, frameRate_);

		// �{�[��������X�L���A�j���[�V����
		if (pParts_[i]->GetSkin() != nullptr)
		{
			pParts_[i]->DrawSkinAnimation(_transfrom, time);
		}
		else  // ���b�V���A�j���[�V����
		{
			pParts_[i]->DrawMeshAnimation(_transfrom, time);
		}
	}
}

void mtgb::Fbx::Release()
{
	SAFE_DELETE_CONTAINER(pParts_);
	SAFE_DESTROY(pFbxScene_);
	SAFE_DESTROY(pFbxManager_);
}

void mtgb::Fbx::CheckNode(FbxNode* _pNode, std::vector<FbxParts*>& _pPartsList)
{
	// �m�[�h�̑������
	FbxNodeAttribute* pNodeAttribute{ _pNode->GetNodeAttribute() };

	if (pNodeAttribute == nullptr)
	{
		return;  // �m�[�h�� nullptr �Ȃ��A
	}

	// ���b�V���̏�񂪓����Ă���Ȃ�
	if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxParts* pParts{ new FbxParts{ _pNode } };
		pParts->Initialize();
		_pPartsList.push_back(pParts);
	}

	// �q�̐�
	int childCount{ _pNode->GetChildCount() };

	// 1���`�F�b�N
	for (int i = 0; i < childCount; i++)
	{
		CheckNode(_pNode->GetChild(i), _pPartsList);
	}
}
