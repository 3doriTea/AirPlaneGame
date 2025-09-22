#pragma once
#include <mtgb.h>

class AutoControlText : public GameObject
{
public:
	AutoControlText();
	~AutoControlText();

	void Update() override;
	void Draw() const override;

	void SetEnabled(const bool _enabled) { enabled_ = _enabled; }
private:
	bool enabled_;  // —LŒø‚©
	ImageHandle hImage_;  // ©“®‘€c’†‚Ì‰æ‘œ
};
