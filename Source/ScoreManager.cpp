#include "ScoreManager.h"

int32_t mtgb::ScoreManager::score_{ 0 };

mtgb::ScoreManager::ScoreManager()
{
}

mtgb::ScoreManager::~ScoreManager()
{
}

void mtgb::ScoreManager::AddScore(int32_t _score)
{
	score_ += _score;
}
