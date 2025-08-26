#pragma once
#include <mtgb.h>

class EndLogo : public GameObject
{
public:
	EndLogo();
	EndLogo(mtgb::WindowContext context);
	~EndLogo();

	void Update() override;
	void Draw() const override;

private:
	FBXModelHandle fModel_;
	Transform* pTransform_;
	mtgb::WindowContext context_;

};