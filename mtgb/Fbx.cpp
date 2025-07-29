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
		&& "fbxImporterの初期化に失敗した @Fbx::Load");

	fbxImporter->Import(pFbxScene_);
	SAFE_DESTROY(fbxImporter);

	FbxGeometryConverter geometryConverter{ pFbxManager_ };

	// アニメーションタイムモードの取得
	frameRate_ = pFbxScene_->GetGlobalSettings().GetTimeMode();

	// 現在のカレントディレクトリを取得
	char defaultCurrentDirectory[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	// カレントディレクトリを移動
	char directory[MAX_PATH]{};
	_splitpath_s(_fileName.c_str(), nullptr, 0, directory, MAX_PATH, nullptr, 0, nullptr, 0);
	SetCurrentDirectory(directory);

	int meshCount{ pFbxScene_->GetSrcObjectCount<FbxMesh>() };
	for (int i = 0; i < meshCount; i++)
	{
		// すべてのメッシュデータを取得できるらしい
		FbxMesh* pMesh{ pFbxScene_->GetSrcObject<FbxMesh>() };

		FbxParts* pParts{ new FbxParts{} };
	}
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
