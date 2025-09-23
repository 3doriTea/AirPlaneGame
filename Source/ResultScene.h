#pragma once
#include <mtgb.h>
#include "Ranking.h"

class TimeLimit;

class ResultScene : public mtgb::GameScene
{
public:
	ResultScene();
	~ResultScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

private:

	TimeLimit* timeLimit_;
	CameraHandleInScene hCamera1_;
	CameraHandleInScene hCamera2_;
	Ranking* ranking_;
	std::vector<int> rankingList_;
	int32_t resultScore_;
	
	// 前のペアのスコア
	int32_t prevPairScore_;
};