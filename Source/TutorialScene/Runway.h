#pragma once
#include <mtgb.h>

class Runway : public GameObject
{
public:
	Runway();
	~Runway();

	void Update() override;
	void Draw() const override;
private:
	FBXModelHandle hModel_;
	Transform* pTransform_;
};

