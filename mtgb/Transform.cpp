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

	GenerateWorldMatrix(&matrixWorld_);           // ワールド行列更新
	GenerateWorldRotationMatrix(&matrixRotate_);  // ワールド回転行列更新
}

void mtgb::Transform::GenerateWorldMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix = DirectX::XMMatrixIdentity();
	GenerateWorldMatrixSelf(_pMatrix);
}

void mtgb::Transform::GenerateWorldRotationMatrix(Matrix4x4* _pMatrix) const
{
	*_pMatrix = DirectX::XMMatrixIdentity();
	GenerateWorldRotMatrixSelf(_pMatrix);
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
	// 親Idが無効値なら nullptr を返す
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

mtgb::Vector3 mtgb::Transform::Forward() const
{
	return Vector3::Forward() * matrixWorldRot_;
}

void mtgb::Transform::GenerateWorldMatrixSelf(Matrix4x4* _pMatrix) const
{
	if (parent != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldMatrixSelf(_pMatrix);
	}
	*_pMatrix *= matrixScale_;
	*_pMatrix *= matrixRotate_;
	*_pMatrix *= matrixTranslate_;
}

void mtgb::Transform::GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const
{
	if (parent != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldRotMatrixSelf(_pMatrix);
	}
	*_pMatrix *= matrixRotate_;
}
