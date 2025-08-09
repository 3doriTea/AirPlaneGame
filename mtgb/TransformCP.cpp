#include "TransformCP.h"


mtgb::TransformCP::TransformCP()
{
}

mtgb::TransformCP::~TransformCP()
{
}

void mtgb::TransformCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			pool_[i].matrixTranslate_ = DirectX::XMMatrixTranslation(
				pool_[i].position.x,
				pool_[i].position.y,
				pool_[i].position.z);
			
			/*pool_[i].matrixRotate_ = DirectX::XMMatrixRotationQuaternion(
				pool_[i].rotate);*/
			
			pool_[i].matrixRotate_ = DirectX::XMMatrixRotationZ(DirectX::XMConvertToRadians(pool_[i].rotate.f[2]));
			pool_[i].matrixRotate_ *= DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(pool_[i].rotate.f[1]));
			pool_[i].matrixRotate_ *= DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(pool_[i].rotate.f[0]));
			
			//pool_[i].matrixRotate_ = DirectX::XMMatrixRotationRollPitchYawFromVector(pool_[i].rotate);

			pool_[i].matrixScale_ = DirectX::XMMatrixScaling(
				pool_[i].scale.x,
				pool_[i].scale.y,
				pool_[i].scale.z);
		}
	}
}
