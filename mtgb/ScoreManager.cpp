#include "ScoreManager.h"
#include "MTImGui.h"

namespace
{
	const int32_t QUATA_SCORE_MAX{ 35 };
	int GAUGE_COUNT{ 30 };  // ƒQ[ƒW”

}

int32_t mtgb::ScoreManager::score_{ 0 };
int32_t mtgb::ScoreManager::quotaScore_{ 0 };

mtgb::ScoreManager::ScoreManager()
{
	quotaScore_ = GAUGE_COUNT;
}

mtgb::ScoreManager::~ScoreManager()
{
}

void mtgb::ScoreManager::Update()
{
	MTImGui::Instance().DirectShow([this]()
		{
			ImGui::LabelText("Score","%d", score_);
		}, "CurrentScore", ShowType::Inspector);
}
void mtgb::ScoreManager::AddScore(uint32_t _score)
{
	score_ += _score;
}

int32_t mtgb::ScoreManager::GetScore()
{
	return score_;
}

void mtgb::ScoreManager::SubtractScore(uint32_t _score)
{
	score_ -= _score;
	score_ = (std::max)(0, score_);
}

int32_t mtgb::ScoreManager::GetQuotaScore()
{
	return quotaScore_;
}

int32_t mtgb::ScoreManager::GetQuotaScoreMax()
{
	return QUATA_SCORE_MAX;
}

void mtgb::ScoreManager::ResetScore()
{
	score_ = 0;
}

bool mtgb::ScoreManager::AchievedQuota()
{
	if (score_ >= quotaScore_)
	{
		return true;
	}
	else
	{
		return false;
	}
}
