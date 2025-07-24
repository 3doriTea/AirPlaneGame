#include "Fbx.h"
#include "ReleaseUtility.h"
#include "FbxParts.h"
#include "DirectX11Draw.h"


mtgb::Fbx::Fbx()
{
}

mtgb::Fbx::~Fbx()
{
	Release();
}

void mtgb::Fbx::Load(const std::string& _fileName)
{
}

void mtgb::Fbx::Draw(Transform* _pTransform, int _frame)
{
	DirectX11Draw::SetBlendMode(BlendMode::Default);

	for (int i = 0; i < pParts_.size(); i++)
	{
		// �A�j���[�V�����^�C���̎p���s����擾����
		FbxTime time;
		time.SetTime(0, 0, 0, _frame, 0, 0, frameRate_);

		if (pParts_[i]->GetSkin())
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
