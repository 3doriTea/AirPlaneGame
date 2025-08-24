#pragma once
#include <mtgb.h>

class EndLogo : public GameObject
{
public:
	EndLogo();
	~EndLogo();

	void Update() override;
	void Draw() const override;

private:
	FBXModelHandle fModel_;
	Transform* pTransform_;

};