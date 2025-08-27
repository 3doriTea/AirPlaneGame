#pragma once
#include <mtgb.h>

class ResultLogo : public GameObject
{
public:
	ResultLogo();
	ResultLogo(mtgb::WindowContext context);
	~ResultLogo();

	void Update() override;
	void Draw() const override;

private:
	FBXModelHandle fModel_;
	Transform* pTransform_;
	mtgb::WindowContext context_;

};