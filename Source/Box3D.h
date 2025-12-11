#pragma once
#include <mtgb.h>

class Box3D : public GameObject
{
public:
	Box3D(const Vector3& _position, const Vector3& _scale, std::string _fileName);

	void Update() override;
	void Draw() const override;
private:
	FBXModelHandle hModel_;
	Transform* pTransform_;
};