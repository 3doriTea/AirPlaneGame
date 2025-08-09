#include "Transform.h"

//mtgb::Transform::Transform()
//{
//}

mtgb::Transform::~Transform()
{
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
	rotate.f[0] += _rotate.x;
	rotate.f[1] += _rotate.y;
	rotate.f[2] += _rotate.z;
	return;

	using DirectX::XMQuaternionMultiply;
	using DirectX::XMQuaternionRotationRollPitchYaw;
	rotate = XMQuaternionMultiply(
		rotate,
		XMQuaternionRotationRollPitchYaw(_rotate.x, _rotate.y, _rotate.z));
}
