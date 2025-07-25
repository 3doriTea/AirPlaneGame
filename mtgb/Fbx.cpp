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

void mtgb::Fbx::Draw(const Transform& _transfrom, int _frame)
{
	DirectX11Draw::SetBlendMode(BlendMode::Default);

	for (int i = 0; i < pParts_.size(); i++)
	{
		// アニメーションタイムの姿勢行列を取得する
		FbxTime time;
		time.SetTime(0, 0, 0, _frame, 0, 0, frameRate_);

		// ボーンがあるスキンアニメーション
		if (pParts_[i]->GetSkin() != nullptr)
		{
			pParts_[i]->DrawSkinAnimation(_transfrom, time);
		}
		else  // メッシュアニメーション
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
	// ノードの属性情報
	FbxNodeAttribute* pNodeAttribute{ _pNode->GetNodeAttribute() };

	if (pNodeAttribute == nullptr)
	{
		return;  // ノードが nullptr なら回帰
	}

	// メッシュの情報が入っているなら
	if (pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxParts* pParts{ new FbxParts{ _pNode } };
		pParts->Initialize();
		_pPartsList.push_back(pParts);
	}

	// 子の数
	int childCount{ _pNode->GetChildCount() };

	// 1つずつチェック
	for (int i = 0; i < childCount; i++)
	{
		CheckNode(_pNode->GetChild(i), _pPartsList);
	}
}
