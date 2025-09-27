#include "DemoScene.h"
#include "../CheckTutorialScene/CheckTutorialScene.h"
#include "../TestScene/TestScene.h"
#include "../SkySphere.h"
#include "../TutorialScene/Runway.h"

#include "HangerUI.h"
#include "CameraTripod.h"

using namespace mtgb;

namespace
{
	static const size_t BUFFER_SIZE{ 1024 };
	mtbin::Byte buffer[BUFFER_SIZE]{};
	const float TO_MOVE_TIME_SEC{ 3.0f };
	const int FULLSCORE_COUNT{ 6 };
	// Draw用の前回のスコアの表示のための定数
	const int PREVPAIRSCORE_INDEX{ 5 };
	const UIParams UI_PARAMS{.depth = 7};

	const int SCORE_POS_X_FIRST{ 110 };
	const int SCORE_POS_X_SECOND{ 1430 };
	const int SCORE_POS_Y{ 260 };
	const int SCORE_MARGIN{ 60 };
	const int PREPAIR_SCORE_Y{ 140 };
	const std::string PREPARE_TEXT{ "前回のペアのスコア: " };
	const int SCORE_SIZE{ 36 };
	const int PREPARE_SCORE_SIZE{ 36 };
}

DemoScene::DemoScene() :
	pHangerUIA_{ nullptr },
	pHangerUIB_{ nullptr },
	toMoveTimeLeft_{ TO_MOVE_TIME_SEC }
{
}

DemoScene::~DemoScene()
{
}

void DemoScene::Initialize()
{
	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Audio::Clear();

	InitializeRanking();
	Instantiate<SkySphere>();

	// 右側の運転手
	CameraTripod* pTripodA = Instantiate<CameraTripod>(Vector3{ 3, 0, 0 }, Quaternion::Identity());
	CameraHandleInScene hCamera1 = RegisterCameraGameObject(pTripodA);
	//EntityId eIdTripodA{ pTripodA->GetEntityId() };

	// 左側の射撃手
	CameraTripod* pTripodB = Instantiate<CameraTripod>(Vector3{ -3, 0, 0 }, Quaternion::Identity());
	CameraHandleInScene hCamera2 = RegisterCameraGameObject(pTripodB);
	//EntityId eIdTripodB{ pTripodB->GetEntityId() };

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);
	
	pHangerUIA_ = Instantiate<HangerUI>(GameObjectLayer::A);
	pHangerUIB_ = Instantiate<HangerUI>(GameObjectLayer::B);

	Runway* pRunway{ Instantiate<Runway>(Vector3{ 0, -10, 20 }, Quaternion::Euler({ 0, 0, 0 })) };
}

void DemoScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<TestScene>();
	}

	if ((pHangerUIA_->IsOpened() && pHangerUIB_->IsOpened())
		|| InputUtil::GetKeyDown(KeyCode::Y))  // デバッグ用 Y キー
	{
		toMoveTimeLeft_ -= Time::DeltaTimeF();
		if (toMoveTimeLeft_ <= 0.0f)
		{
			Game::System<SceneSystem>().Move<CheckTutorialScene>();
		}
	}
	else
	{
		toMoveTimeLeft_ = TO_MOVE_TIME_SEC;
	}
}

void DemoScene::Draw() const
{
	if (!rankingList_.empty())
	{
		WindowContext ctx = CurrContext();
		int x = 0;
		if (ctx == WindowContext::First)
		{
			x = SCORE_POS_X_SECOND;
		}
		else if (ctx == WindowContext::Second)
		{
			x = SCORE_POS_X_FIRST;
		}
		Vector2F scorePos = GenDrawScreenFrom(Vector2F{ x,SCORE_POS_Y });
		int scoreMargin = GenDrawScreenFontSize(SCORE_MARGIN);
		int scoreSize = GenDrawScreenFontSize(PREPARE_SCORE_SIZE);
		for (auto i = 0; i < rankingList_.size() - 1; ++i)
		{
			Draw::ImmediateText(std::to_string(i + 1) + "位: " + std::to_string(rankingList_[i]),
				{ scorePos.x, scorePos.y + i * scoreMargin }, SCORE_SIZE, TextAlignment::topLeft, UI_PARAMS);
		}

		Draw::ImmediateText(PREPARE_TEXT + std::to_string(rankingList_[PREVPAIRSCORE_INDEX]), { scorePos.x, PREPAIR_SCORE_Y }, scoreSize, TextAlignment::topLeft, UI_PARAMS);
		//Draw::ImmediateText(std::to_string(rankingList_[PREVPAIRSCORE_INDEX]), { static_cast<float>(PREPARE_TEXT.size() * scoreSize), PREPAIR_SCORE_Y}, PREPARE_SCORE_SIZE, TextAlignment::topLeft, UI_PARAMS);
	}
	

}

void DemoScene::End()
{
}

void DemoScene::InitializeRanking()
{
	ranking_ = new Ranking();
	// falseの場合はランキング更新せず、ファイルの有無だけ確認
	struct _stat s;
	mtbin::MemoryStream ms{ buffer, BUFFER_SIZE };
	int rc = _stat("ranking.dat", &s);
	if (rc == -1)
	{
		std::vector<int> initData(FULLSCORE_COUNT, 0);
		rankingList_ = initData; // ←ここで0埋めを反映
	}
	else
	{
		// ファイルあれば読み込んでサイズ確保
		ranking_->LoadFileToMemoryStream("ranking.dat", ms);
		rankingList_ = ranking_->GetRankingList();
		if (rankingList_.size() < FULLSCORE_COUNT)
		{
			rankingList_.resize(FULLSCORE_COUNT, 0);
		}
	}
	prevPairScore_ = 0; // またはファイル内スコアの初期化処理

}
