#pragma once
#include <mtgb.h>
#include <vector>

class ControlTower : public mtgb::GameObject
{
public:
	ControlTower(const EntityId _plane);
	~ControlTower();

	void Update() override; 
	void Draw() const override;
	
private:
	Transform* pPlayerPlaneTransform_;
	float detectionRagius_;// プレイヤーを原点にして検出をする球の半径
	void DetectionEnemy();
	std::vector<EntityId> detectedEnemys_;// 検出した敵のID
};
