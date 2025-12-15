#pragma once
#include <mtgb.h>

class AltitudeWarn : public GameObject
{
public:
	AltitudeWarn();
	~AltitudeWarn();

	void Update() override;
	void Draw() const override;

	void SetEnabled(const bool _enabled) { enabled_ = _enabled; }

private:
	bool enabled_;  // —LŒø‚©
	ImageHandle hImage_;  // Œx‚Ì‰æ‘œ
};
