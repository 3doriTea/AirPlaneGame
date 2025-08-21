#pragma once
#include <mtgb.h>

class SpeedMeter : public mtgb::GameObject
{
public:
	SpeedMeter();
	~SpeedMeter();

	void Update() override;
	void Draw() const override;

private:
	float speed_;
};
