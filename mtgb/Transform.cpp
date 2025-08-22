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
	matrixRotate_ = XMMatrixRotationQuaternion(rotate);  // TODO: ここでマトリクススケールが-1になる
	matrixScale_ = XMMatrixScaling(scale.x, scale.y, scale.z);

	if (matrixRotate_.r[0].m128_f32[0] < 0)
	{
		
		printf("");
	}

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

	return XMQuaternionMultiply(rotate, XMQuaternionRotationMatrix(matrixWorldRot_));
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


	if (_pMatrix->r[0].m128_f32[0] < 0)
	{
		printf("");
	}

}

void mtgb::Transform::GenerateWorldRotMatrixSelf(Matrix4x4* _pMatrix) const
{
	if (parent != INVALD_ENTITY)
	{
		GetParent()->GenerateWorldRotMatrixSelf(_pMatrix);
	}
	*_pMatrix *= matrixRotate_;
}
