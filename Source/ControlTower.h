#pragma once
#include <mtgb.h>
#include <vector>
#include <tuple>
class ControlTower : public mtgb::GameObject
{
public:
	ControlTower();
	~ControlTower();

	void Update() override; 
	void Draw() const override;
	void SetGunner(EntityId _id, WindowContext _context);
	void SetPilot(EntityId _id, WindowContext _context);
private:
	std::pair<Transform*, WindowContext> pGunner_;
	std::pair<Transform*, WindowContext> pPilot_;
	Transform* pPlayerPlaneTransform_;
	float detectionRadius_;// プレイヤーを原点にして検出をする球の半径
	std::string DetectionEnemy(Transform* _transform, WindowContext _context);
	std::vector<EntityId> detectedEnemies_;// 検出した敵のID
};
