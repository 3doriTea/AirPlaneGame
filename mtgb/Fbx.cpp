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

void mtgb::Fbx::Initialize()
{
}

void mtgb::Fbx::Update()
{
}

int mtgb::Fbx::Load(const std::string& _fileName)
{
	Fbx& instance{ Game::System<Fbx>() };

	instance.pFbxManager_ = FbxManager::Create();
	instance.pFbxScene_ = FbxScene::Create(instance.pFbxManager_, "fbxscene");
	FbxString fileName{ _fileName.c_str() };
	FbxImporter* fbxImporter{ FbxImporter::Create(instance.pFbxManager_, "imp") };

	massert(fbxImporter->Initialize(fileName.Buffer(), -1, instance.pFbxManager_->GetIOSettings())
		&& "fbxImporterの初期化に失敗した @Fbx::Load");

	fbxImporter->Import(instance.pFbxScene_);
	SAFE_DESTROY(fbxImporter);

	FbxGeometryConverter geometryConverter{ instance.pFbxManager_ };

	// アニメーションタイムモードの取得
	instance.frameRate_ = instance.pFbxScene_->GetGlobalSettings().GetTimeMode();

	// 現在のカレントディレクトリを取得
	char defaultCurrentDirectory[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, defaultCurrentDirectory);

	// カレントディレクトリを移動
	char directory[MAX_PATH]{};
	_splitpath_s(_fileName.c_str(), nullptr, 0, directory, MAX_PATH, nullptr, 0, nullptr, 0);
	SetCurrentDirectory(directory);

	int meshCount{ instance.pFbxScene_->GetSrcObjectCount<FbxMesh>() };
	for (int i = 0; i < meshCount; i++)
	{
		FbxMesh* pMesh = instance.pFbxScene_->GetSrcObject<FbxMesh>(i);
		if (pMesh == nullptr) continue;

		FbxNode* pNode = pMesh->GetNode();
		if (pNode == nullptr) continue;

		// 作成前にノードのメッシュ有無確認
		if (pNode->GetMesh() == nullptr) continue;

		FbxParts* pParts = new FbxParts(pNode);
		pParts->Initialize();
		instance.pParts_.push_back(pParts);
	}
	return (int)instance.pParts_.size() - 1;
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


mtgb::Vector3 mtgb::Fbx::GetBonePosition(std::string _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pParts_.size(); i++)
	{
		if (pParts_[i]->TryGetBonePosition(_boneName, &position_))
			break;
	}
	return position_;
}

mtgb::Vector3 mtgb::Fbx::GetAnimBonePosition(std::string _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pParts_.size(); i++)
	{
		if (pParts_[i]->TryGetBonePositionAtNow(_boneName, &position_))
			break;
	}
	return position_;
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
		FbxParts* pParts = new FbxParts(_pNode);
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
