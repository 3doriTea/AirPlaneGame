#pragma once
#include <mtgb.h>

class Reticle : public mtgb::GameObject
{
public:
	Reticle();
	~Reticle();

	void Update() override;
	void Draw() const override;

private:
	ImageHandle hImage_ = 0;
	Transform* pTransform_;
};