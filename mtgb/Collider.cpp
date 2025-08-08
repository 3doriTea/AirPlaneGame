#include "Collider.h"
#include "Transform.h"
#include "DirectXMath.h"
#include "Matrix4x4.h"

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
			Vector3 worldPosition{ pTransform_->position_ * matrix };

			_other.pTransform_->GenerateWorldMatrix(&matrix);
			Vector3 otherWorldPosition{ pTransform_->position_ * matrix };

			float distance{ (otherWorldPosition - worldPosition).Size() };
			float hitDistance{ sphere_.radius_ + _other.sphere_.radius_ };

			// �������o���̋��̔��a������������Γ������Ă���
			return (distance <= hitDistance);
		}
		else if (type_ == TYPE_CAPSULE)
		{
			// TODO: �J�v�Z�����m�̓����蔻��
		}
	}
	else
	{
		if (type_ == TYPE_SPHERE)
		{
			// TODO: ���Ƌ��̓����蔻��
		}
		else if (type_ == TYPE_CAPSULE)
		{
			// TODO: �J�v�Z���Ƌ��̓����蔻��
		}
	}

	return false;
}
