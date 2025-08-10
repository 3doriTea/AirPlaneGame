#include "Transform.h"

//mtgb::Transform::Transform()
//{
//}

mtgb::Transform::~Transform()
{
}

void mtgb::Transform::Compute()
{
	using DirectX::XMMatrixTranslation;
	using DirectX::XMMatrixRotationQuaternion;
	using DirectX::XMConvertToRadians;
	using DirectX::XMMatrixScaling;

	matrixTranslate_ = XMMatrixTranslation(position.x, position.y, position.z);
	matrixRotate_ = XMMatrixRotationQuaternion(rotate);
	matrixScale_ = XMMatrixScaling(scale.x, scale.y, scale.z);
}

void mtgb::Transform::GenerateWorldMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixScale_;
	*_pMatrix *= matrixRotate_;
	*_pMatrix *= matrixTranslate_;
	if (parent != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldMatrix(_pMatrix);
	}
}

void mtgb::Transform::GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixRotate_;
	if (parent != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldMatrix(_pMatrix);
	}
}

void mtgb::Transform::GenerateParentMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixScale_;
	*_pMatrix *= matrixRotate_;
	*_pMatrix *= matrixTranslate_;
}

void mtgb::Transform::GenerateParentRotationMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix *= matrixRotate_;
}

mtgb::Transform* mtgb::Transform::GetParent() const
{
	// êeIdÇ™ñ≥å¯ílÇ»ÇÁ nullptr Çï‘Ç∑
	if (parent == INVALD_ENTITY)
	{
		return nullptr;
	}
	else
	{
		return &Transform::Get(parent);
	}
}

void mtgb::Transform::Rotation(const Vector3& _rotate)
{
	using DirectX::XMQuaternionMultiply;
	using DirectX::XMQuaternionRotationRollPitchYaw;
	using DirectX::XMQuaternionNormalize;

	//rotate
	rotate = XMQuaternionMultiply(
		rotate,
		XMQuaternionRotationRollPitchYaw(_rotate.x, _rotate.y, _rotate.z));
}
