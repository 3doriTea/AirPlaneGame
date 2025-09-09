#include "ResultScene.h"
#include "SkySphere.h"
#include "TestScene/TestScene.h"
#include "Draw.h"
#include "ResultLogo.h"
#include <iostream>
#include <sys/stat.h>
#include "TimeLimit.h"
#include "TextBox.h"


#include "../Source/TestScene/PlayerPlane.h"
#include "../Source/TestScene/PlayerGunner.h"
#include "../Source/TestScene/PlayerPilot.h"

namespace
{
	static const size_t BUFFER_SIZE{ 1024 };
	mtbin::Byte* buffer = new mtbin::Byte[BUFFER_SIZE];
	int maxRankingCount{ 5 };
    Vector2F textPos_{ 400.0f, 400.0f };
}

ResultScene::ResultScene()
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Initialize()
{
	hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::First));
	hCamera2_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 0, 0, -10 }, WindowContext::Second));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1_);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2_);

    ScoreManager::ResetScore();
    

	Audio::Clear();

	Instantiate<SkySphere>();
//	Instantiate<ResultText>();
//	Instantiate<TextBox>("tekita", 0.1f);
	Instantiate<ResultLogo>();

    timeLimit_ = Instantiate<TimeLimit>(10.0f);
	timeLimit_->RegisterOnEndTimerCallback([]()
		{
			Game::System<SceneSystem>().Move<TestScene>();
		});

	mtbin::MemoryStream ms{ buffer, BUFFER_SIZE };

    ranking_ = new Ranking();

    struct _stat s;
    int rc = _stat("ranking.dat", &s);
    if (rc == -1)
    {
        // ファイルが存在しない場合、0で初期化
        std::vector<int> initData(maxRankingCount, 0);
        for (const auto& score : initData)
        {
            ms.Write<int>(score);
        }
        ranking_->SaveMemoryStreamToFile("ranking.dat", ms, sizeof(int) * maxRankingCount);
        ms.Seek(mtbin::MemoryStream::SeekDir::Head);

        rankingList_ = initData; // ←ここで0埋めを反映
    }
    else
    {
        // ファイルが存在する場合、読み込み
        ranking_->LoadFileToMemoryStream("ranking.dat", ms);
        rankingList_ = ranking_->GetRankingList();
    }

    // 仮のスコア
    ScoreManager::AddScore(1000);
    int32_t testScore = ScoreManager::GetScore();
    ranking_->UpdateRanking(rankingList_, testScore);

    // 保存
    ms.Seek(mtbin::MemoryStream::SeekDir::Head);
    ms.Write(rankingList_.data(), static_cast<int>(rankingList_.size()));
    ranking_->SaveMemoryStreamToFile("ranking.dat", ms, sizeof(int) * rankingList_.size());
}

void ResultScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
	//	Game::System<SceneSystem>().Move<TestScene>();
        timeLimit_->StartTimer();
	}
}

void ResultScene::Draw() const
{
	Draw::ImmediateText("あなたのスコア", { 0, 100 }, 48, TextAlignment::center);
	for (auto i = 0; i < rankingList_.size(); ++i)
	{
		Draw::ImmediateText(std::to_string(i + 1) + "位: " + std::to_string(rankingList_[i]),
			{ 0, 160 + i * 40 }, 32, TextAlignment::center);
	}

    Draw::ImmediateText("Tキーを押したら10秒後にタイトルへ戻ります"
		, { 0, 50 }, 16, TextAlignment::center);
}

void ResultScene::End()
{
}
