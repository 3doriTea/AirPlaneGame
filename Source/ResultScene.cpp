#include "ResultScene.h"
#include "SkySphere.h"
#include "TestScene/TestScene.h"
#include "Draw.h"
#include "ResultLogo.h"
#include <iostream>
#include <sys/stat.h>
#include <string>
#include "TimeLimit.h"
#include "TextBox.h"


#include "../Source/TestScene/PlayerPlane.h"
#include "../Source/TestScene/PlayerGunner.h"
#include "../Source/TestScene/PlayerPilot.h"
#include "../Source/DemoScene/DemoScene.h"
using namespace mtgb;

namespace
{
	static const size_t BUFFER_SIZE{ 1024 };
	mtbin::Byte buffer[BUFFER_SIZE]{};
	// ランキング分のカウント（1～5位）
	const int MAXRANKING_COUNT{ 5 };
	// 前回のペアのスコア含めたカウント
	const int FULLSCORE_COUNT{ 6 };
	// Draw用の前回のスコアの表示のための定数
	const int PREVPAIRSCORE_INDEX{ 5 };
	Vector2F textPos_{ 400.0f, 400.0f };
	const UIParams UI_PARAMS{};
}

ResultScene::ResultScene()
{
	
}

ResultScene::~ResultScene()
{
	SAFE_DELETE(ranking_);
}

void ResultScene::Initialize()
{
	hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::First));
	hCamera2_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::Second));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1_);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2_);

	Audio::Clear();

	Instantiate<SkySphere>();
//	Instantiate<ResultText>();
//	Instantiate<TextBox>("tekita", 0.1f);
	Instantiate<ResultLogo>();

	timeLimit_ = Instantiate<TimeLimit>(10.0f);
	timeLimit_->RegisterOnEndTimerCallback([]()
		{
			Game::System<SceneSystem>().Move<DemoScene>();
		});
	timeLimit_->StartTimer();

	mtbin::MemoryStream ms{ buffer, BUFFER_SIZE };


	ranking_ = new Ranking();

	if (ScoreManager::AchievedQuota() == true)
	{
		resultScore_ = ScoreManager::GetScore();
		struct _stat s;
		int rc = _stat("ranking.dat", &s);
		if (rc == -1)
		{
			// ファイルが存在しない場合、0で初期化
			std::vector<int> initData(FULLSCORE_COUNT, 0);
			for (const auto& score : initData)
			{
				ms.Write<int>(score);
			}
			ranking_->SaveMemoryStreamToFile("ranking.dat", ms, sizeof(int) * FULLSCORE_COUNT);
			ms.Seek(mtbin::MemoryStream::SeekDir::Head);

			prevPairScore_ = 0;
			rankingList_ = initData; // ←ここで0埋めを反映
		}
		else
		{
			// ファイルが存在する場合、読み込み
			ranking_->LoadFileToMemoryStream("ranking.dat", ms);
			rankingList_ = ranking_->GetRankingList();
			// 6個分に満たないなら拡張
			if (rankingList_.size() < FULLSCORE_COUNT)
			{
				rankingList_.resize(FULLSCORE_COUNT, 0);
			}

			prevPairScore_ = rankingList_[PREVPAIRSCORE_INDEX];
			rankingList_[PREVPAIRSCORE_INDEX] = resultScore_;
		}


		// ランキング更新
		std::vector<int> rankSubset(rankingList_.begin(), rankingList_.begin() + MAXRANKING_COUNT);
		ranking_->UpdateRanking(rankSubset, resultScore_);

		// 更新後の上位5位をrankingList_に反映
		for (int i = 0; i < MAXRANKING_COUNT; ++i)
		{
			rankingList_[i] = rankSubset[i];
		}

		// 保存
		ms.Seek(mtbin::MemoryStream::SeekDir::Head);
		ms.Write(rankingList_.data(), static_cast<int>(rankingList_.size()));

		// 前回のペアのスコアを最後に挿入
		//	rankingList_.push_back(prevPairScore_);

		ranking_->SaveMemoryStreamToFile("ranking.dat", ms, sizeof(int) * FULLSCORE_COUNT);
	}
	else
	{
		// falseの場合はランキング更新せず、ファイルの有無だけ確認
		struct _stat s;
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
}

void ResultScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<TestScene>();
	}
}

void ResultScene::Draw() const
{
	// 自分のスコアを表示
	// ノルマ未達成なら失敗、達成なら成功のテキストを描画
	if (ScoreManager::AchievedQuota() == false)
	{
		Draw::ImmediateText("ノルマ達成ならず…", { 170, 390 }, 48, TextAlignment::topLeft, UI_PARAMS);
	}
	else
	{
		Draw::ImmediateText("あなたのスコア：", { 170, 390 }, 48, TextAlignment::topLeft, UI_PARAMS);
		Draw::ImmediateText(std::to_string(resultScore_), { 800, 390 }, 48, TextAlignment::topLeft, UI_PARAMS);
	}
	
	for (auto i = 0; i < rankingList_.size() - 1; ++i)
	{
		Draw::ImmediateText(std::to_string(i + 1) + "位: " + std::to_string(rankingList_[i]),
			{ 0, 160 + i * 40 }, 32, TextAlignment::center, UI_PARAMS);
	}

	Draw::ImmediateText("前回のペアのスコア: ", {170, 300}, 48, TextAlignment::topLeft, UI_PARAMS);
	Draw::ImmediateText(std::to_string(rankingList_[PREVPAIRSCORE_INDEX]), {800, 300}, 48, TextAlignment::topLeft, UI_PARAMS);

	/*Draw::ImmediateText("Tキーを押したら10秒後にタイトルへ戻ります"
		, { 0, 50 }, 16, TextAlignment::center);*/
}

void ResultScene::End()
{
}
