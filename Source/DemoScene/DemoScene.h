#pragma once
#include <mtgb.h>
#include "../Source/Ranking.h"

class HangerUI;

class DemoScene : public mtgb::GameScene
{
public:
	DemoScene();
	~DemoScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
	void InitializeRanking();

private:
	Ranking* ranking_;
	std::vector<int> rankingList_;
	int32_t resultScore_;

	// 前のペアのスコア
	int32_t prevPairScore_;
	HangerUI* pHangerUIA_;  // 格納庫UI右ゲームオブジェクトのポインタ
	HangerUI* pHangerUIB_;  // 格納庫UI左ゲームオブジェクトのポインタ
	float toMoveTimeLeft_;  // 両方のドアが開いてから遷移するまでの残り時間
};
