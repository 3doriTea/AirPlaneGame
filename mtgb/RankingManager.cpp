#include "RankingManager.h"
#include "ReleaseUtility.h"
#include <iostream>
#include <sys/stat.h>

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

RankingManager::RankingManager()
{
}

RankingManager::~RankingManager()
{
	SAFE_DELETE(ranking_);
}

void RankingManager::Initialize()
{
}

void RankingManager::Release()
{
}
