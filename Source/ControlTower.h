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
	void SetGunner(EntityId _id, CameraHandleInScene _hCamera);
	void SetPilot(EntityId _id, CameraHandleInScene _hCamera);
private:
	std::pair<Transform*, CameraHandleInScene> pGunner_;
	std::pair<Transform*, CameraHandleInScene> pPilot_;
	Transform* pPlayerPlaneTransform_;
	float detectionRadius_;// プレイヤーを原点にして検出をする球の半径
	std::string DetectionEnemy(Transform* _transform, CameraHandleInScene _hCamera);
	std::vector<EntityId> detectedEnemies_;// 検出した敵のID
};
