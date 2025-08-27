#pragma once
#include <mtgb.h>
#include <vector>
#include "Library/YzTrailEmitter.h"
#include "Trail.h"


class TrailEmitterSystem : public mtgb::ISystem
{
public:
	TrailEmitterSystem();
	~TrailEmitterSystem();

	void Initialize() override;
	void Update() override;
	void Render();

	void Release() override;

private:
	std::vector<yz::TrailEmitter> trails_;
	float width_;  // トレイルの横幅
	float pointsMax_;  // トレイルの最大頂点数

	Trail trail_;
};
