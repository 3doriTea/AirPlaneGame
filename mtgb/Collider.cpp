#include "Collider.h"
#include "Transform.h"
#include "DirectXMath.h"
#include "Matrix4x4.h"
#include "Draw.h"
#include "Debug.h"

mtgb::Collider::Collider(const EntityId _entityId) :
	IComponent{ _entityId },
	pTransform_{ &Transform::Get(_entityId) }
{
}

mtgb::Collider::~Collider()
{
}

bool mtgb::Collider::IsHit(const Collider& _other) const
{
	using DirectX::XMVector3TransformCoord;

	static Matrix4x4 matrix{};
	if (type_ == _other.type_)
	{
		if (type_ == TYPE_SPHERE)
		{
			pTransform_->GenerateWorldMatrix(&matrix);
			Vector3 worldPosition{ sphere_.offset_ * matrix };

			_other.pTransform_->GenerateWorldMatrix(&matrix);
			Vector3 otherWorldPosition{ _other.sphere_.offset_ * matrix };

			float distance{ (otherWorldPosition - worldPosition).Size() };
			float hitDistance{ sphere_.radius_ + _other.sphere_.radius_ };

			//LOGF("distance: %f <= %f :hit\n", distance, hitDistance);

			// 距離が双方の球の半径よりも小さければ当たっている
			return (distance <= hitDistance);

			//DirectX::ContainmentType type{ computeSphere_.Contains(_other.computeSphere_) };
			//return type != DirectX::ContainmentType::DISJOINT;
		}
		else if (type_ == TYPE_CAPSULE)
		{
			// TODO: カプセル同士の当たり判定
		}
	}
	else
	{
		if (type_ == TYPE_SPHERE)
		{
			// TODO: 球とカプセルの当たり判定
		}
		else if (type_ == TYPE_CAPSULE)
		{
			// TODO: カプセルと球の当たり判定
		}
	}

	return false;
}

void mtgb::Collider::Draw() const
{
	static Transform copyTransform{};
	switch (type_)
	{
	case mtgb::Collider::TYPE_SPHERE:
		copyTransform = *pTransform_;
		copyTransform.scale = Vector3::One() * sphere_.radius_;
		copyTransform.position += sphere_.offset_;
		copyTransform.Compute();
		Draw::SetShaderOnce(ShaderType::Debug3D);
		Draw::FBXModel(hSphereModel_, copyTransform, 0);
		break;
	case mtgb::Collider::TYPE_CAPSULE:
		break;
	default:
		break;
	}
}

mtgb::FBXModelHandle mtgb::Collider::hSphereModel_{ mtgb::INVALID_HANDLE };
