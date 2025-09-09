#include "ScoreManager.h"
#include "MTImGui.h"

int32_t mtgb::ScoreManager::score_{ 0 };

mtgb::ScoreManager::ScoreManager()
{
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
void mtgb::ScoreManager::AddScore(int32_t _score)
{
	score_ += _score;
}
