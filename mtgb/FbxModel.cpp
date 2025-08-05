#include "FbxModel.h"
#include "FbxParts.h"
#include "ReleaseUtility.h"
#include "Fbx.h"
#include "MTAssert.h"


mtgb::FbxModel::FbxModel() :
	animationSpeed_{ 1.0f },
	startFrame_{ 0 },
	endFrame_{ 0 },
	frameRate_{ FbxTime::EMode::eFrames60 },
	pFbxScene_{ nullptr }
{
}

mtgb::FbxModel::~FbxModel()
{
}

void mtgb::FbxModel::Load(const std::string& _fileName)
{
	massert(pFbxScene_ == nullptr  // まだ読み込まれていない
		&& "既にFbxModelは読み込まれているよ！ @FbxModel::Load");

	FbxManager* pFbxManager{ Game::System<Fbx>().GetFbxManager() };

	pFbxScene_ = FbxScene::Create(pFbxManager, "fbxscene");
	FbxString fileName{ _fileName.c_str() };
	FbxImporter* fbxImporter{ FbxImporter::Create(pFbxManager, "imp") };

	FbxIOSettings* ios = FbxIOSettings::Create(pFbxManager, IOSROOT);
	pFbxManager->SetIOSettings(ios);
	if (!fbxImporter->Initialize(fileName.Buffer(), -1, ios)) {
		MessageBoxA(NULL, fbxImporter->GetStatus().GetErrorString(), "FBX Import Error", MB_OK);
		// もしくはログ出力
	}
	//none of the registered readers can process the file

	fileName_ = _fileName;


	char str[MAX_PATH]{};
	GetCurrentDirectory(MAX_PATH, str);

	bool succeed{ false };
	//succeed = fbxImporter->Initialize(fileName.Buffer(), -1);
	//massert(succeed	&& "fbxImporterの初期化に失敗した @Fbx::Load");
	succeed = fbxImporter->Initialize(fileName.Buffer(), -1, pFbxManager->GetIOSettings());
	massert(succeed && "fbxImporterの初期化に失敗した @Fbx::Load");

	succeed = fbxImporter->Import(pFbxScene_);
	massert(succeed && "読み込みに失敗した @Fbx::Load");

	SAFE_DESTROY(fbxImporter);  // インポータは解放

	// 3角ポリゴン
	FbxGeometryConverter geometryConverter{ pFbxManager };

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
		FbxMesh* pMesh = pFbxScene_->GetSrcObject<FbxMesh>(i);
		if (pMesh == nullptr) continue;

		FbxNode* pNode = pMesh->GetNode();
		if (pNode == nullptr) continue;

		// 作成前にノードのメッシュ有無確認
		if (pNode->GetMesh() == nullptr) continue;

		FbxParts* pParts = new FbxParts(pNode);
		pParts->Initialize();
		pParts_.push_back(pParts);
	}

	SetCurrentDirectory(defaultCurrentDirectory);

}

void mtgb::FbxModel::Draw(const Transform& _transfrom, int _frame)
{
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

void mtgb::FbxModel::Release()
{
}

mtgb::Vector3 mtgb::FbxModel::GetBonePosition(std::string _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pParts_.size(); i++)
	{
		if (pParts_[i]->TryGetBonePosition(_boneName, &position_))
			break;
	}
	return position_;
}

mtgb::Vector3 mtgb::FbxModel::GetAnimBonePosition(std::string _boneName)
{
	Vector3 position_ = Vector3(0, 0, 0);
	for (int i = 0; i < pParts_.size(); i++)
	{
		if (pParts_[i]->TryGetBonePositionAtNow(_boneName, &position_))
			break;
	}
	return position_;
}

void mtgb::FbxModel::CheckNode(FbxNode* _pNode, std::vector<FbxParts*>& _pPartsList)
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

