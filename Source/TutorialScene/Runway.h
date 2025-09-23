#pragma once
#include <mtgb.h>

class Runway : public GameObject
{
public:
	Runway(const Vector3 _position, const Quaternion _quaternion);
	~Runway();

	Transform* GetTransform() { return pTransform_; }

	void Update() override;
	void Draw() const override;
private:
	FBXModelHandle hModel_;
	Transform* pTransform_;
};
