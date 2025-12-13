#pragma once
#include <mtgb.h>

/// <summary>
/// FBXの3Dモデルを描画するエンティティ
/// </summary>
class FBXModelEntity : public GameObject
{
public:
	FBXModelEntity(const Vector3& _position, const Vector3& _scale, std::string _modelFile);

	void Update() override;
	void Draw() const override;
private:
	FBXModelHandle hModel_;
	Transform* pTransform_;
};