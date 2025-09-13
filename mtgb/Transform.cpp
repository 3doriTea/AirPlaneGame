#include "Transform.h"

//mtgb::Transform::Transform()
//{
//	Compute();
//}

mtgb::Transform::~Transform()
{
}

void mtgb::Transform::Compute()
{
	using DirectX::XMMatrixTranslation;
	using DirectX::XMMatrixRotationQuaternion;
	using DirectX::XMQuaternionNormalize;
	using DirectX::XMConvertToRadians;
	using DirectX::XMMatrixScaling;

	matrixTranslate_ = XMMatrixTranslation(position.x, position.y, position.z);
	matrixRotate_ = XMMatrixRotationQuaternion(
		XMQuaternionNormalize(rotate));  // TODO: ここでマトリクススケールが-1になる
	matrixScale_ = XMMatrixScaling(scale.x, scale.y, scale.z);

	GenerateWorldMatrix(&matrixWorld_);             // ワールド行列更新
	GenerateWorldRotationMatrix(&matrixWorldRot_);  // ワールド回転行列更新
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

void mtgb::Transform::Rotation(const Vector3& _axis, const float _angle)
{
	using DirectX::XMQuaternionRotationAxis;

	rotate *= XMQuaternionRotationAxis(_axis, _angle);
}

mtgb::Vector3 mtgb::Transform::Forward() const
{
	return Vector3::Forward() * matrixWorldRot_;
}

mtgb::Quaternion mtgb::Transform::GetWorldRotate() const
{
	using DirectX::XMQuaternionRotationMatrix;
	using DirectX::XMQuaternionMultiply;
	using DirectX::XMQuaternionIdentity;

	return XMQuaternionMultiply(XMQuaternionRotationMatrix(matrixWorldRot_), XMQuaternionIdentity());
}

void mtgb::Transform::GenerateWorldMatrixSelf(Matrix4x4* _pMatrix) const
{
	if (parent != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldMatrixSelf(_pMatrix);
		*_pMatrix = matrixScale_ * matrixRotate_ * matrixTranslate_ * (*_pMatrix);
	}
	else
	{
		*_pMatrix = matrixScale_ * matrixRotate_ * matrixTranslate_;
	}
}

void mtgb::Transform::GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const
{
	if (parent != INVALD_ENTITY)
	{
		Matrix4x4 mWorldRotParent{};
		GetParent()->GenerateWorldRotMatrixSelf(&mWorldRotParent);
		*_pMatrix = matrixRotate_ * mWorldRotParent;
	}
	else
	{
		*_pMatrix = matrixRotate_;
	}
}
