#pragma once
#include <mtgb.h>

class Reticle : public mtgb::GameObject
{
public:
	Reticle();
	Reticle(mtgb::WindowContext context);
	~Reticle();

	void Update() override;
	void Draw() const override;

private:
	ImageHandle hImage_;
	Transform* pTransform_;
	RectInt reticlePos_;
	mtgb::WindowContext context_;
};
