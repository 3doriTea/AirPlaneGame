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
	// BoundingSphereを初期化
	computeSphere_ = DirectX::BoundingSphere();
}

mtgb::Collider::~Collider()
{
}

void mtgb::Collider::InitializeBoundingSphere()
{
	if (type_ == TYPE_SPHERE)
	{
		// Transformから現在の位置を取得してBoundingSphereを更新
		static Matrix4x4 matrix{};
		pTransform_->GenerateWorldMatrix(&matrix);
		Vector3 worldPosition = sphere_.offset_ * matrix;
		
		computeSphere_.Center = DirectX::XMFLOAT3(worldPosition.x, worldPosition.y, worldPosition.z);
		computeSphere_.Radius = sphere_.radius_;
	}
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

			//return computeSphere_.Intersects(_other.computeSphere_);

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

bool mtgb::Collider::IsHit(const Vector3& _origin, const Vector3& _dir, float* dist)
{
	using namespace DirectX;
	// まずBoundingSphereを最新状態に更新
	const_cast<Collider*>(this)->InitializeBoundingSphere();
	
	// 方向ベクトルを正規化（元のベクトルは保持）
	Vector3 normalizedDir = Vector3::Normalize(_dir);
	
	if (!DirectX::Internal::XMVector3IsUnit(normalizedDir))
	{
		return false;
	}
	
	XMVECTOR vCenter =  XMLoadFloat3(&computeSphere_.Center);
	XMVECTOR vRadius =  XMVectorReplicatePtr(&computeSphere_.Radius);

	//球の中心からレイの原点へのベクトル
	XMVECTOR l = XMVectorSubtract(vCenter, _origin);

	//lをレイの方向に射影したスカラー
	//球の中心とレイの最も近い点の距離
	XMVECTOR s = XMVector3Dot(l, normalizedDir);

	//球の中心からレイの原点への距離の二乗
	XMVECTOR l2 = XMVector3Dot(l, l);

	//半径の二乗
	XMVECTOR r2 = XMVectorMultiply(vRadius, vRadius);

	//球の中心からレイへの垂線の二乗
	//三平方の定理でいうとlが斜辺にあたる
	
	XMVECTOR m2 = XMVectorNegativeMultiplySubtract(s, s, l2);
	
	
	XMVECTOR NoIntersection;

	//もしレイの原点が球の外側、かつ
	//球の中心がレイの原点の後ろにあるならば接触していない
	NoIntersection = XMVectorAndInt(
		//射影ベクトルが負ならレイの方向と反対、レイの原点より後ろにある
		XMVectorLess(s, XMVectorZero())
		//l2の方が大きいなら球の外側にある
		, XMVectorGreater(l2, r2));

	//球の中心からレイへの垂線が球の半径よりも大きいか
	//レイの最も近い地点が球の外側にあるか


	if (XMVector4Greater(m2, r2))
	{
		return false;
	}
	//NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(m2, r2));


	//衝突しているとして、最も近い地点を計算

	//球の中心とレイの最も近い点から球の表面までの距離
	XMVECTOR q = XMVectorSqrt(XMVectorSubtract(r2, m2));

	//r2-m2が負の値の場合はqに0を入れる
	XMVECTOR mask = XMVectorGreater(m2, r2);
	XMVECTOR zero = XMVectorZero();
	//maskがtrueの場合はzero、falseの場合はq
	q = XMVectorSelect(q, zero, mask);

	//レイの原点から球との交点までの距離
	//手前
	XMVECTOR t1 = XMVectorSubtract(s, q);
	//奥
	XMVECTOR t2 = XMVectorAdd(s, q);

	//レイの原点が球の内側か
	XMVECTOR originInside = XMVectorLessOrEqual(l2, r2);
	
	//第三引数がtrueなら第二引数、falseなら第一引数
	XMVECTOR t = XMVectorSelect(t1, t2, originInside);

	if (XMVector4NotEqualInt(NoIntersection, XMVectorTrueInt()))
	{
		DirectX::XMStoreFloat(dist, t);
		return true;
	}

	return false;
}

bool mtgb::Collider::IsHit(const Vector3& _center, float _radius) const
{
	static Matrix4x4 matrix{};

	if (type_ == TYPE_SPHERE)
	{
		pTransform_->GenerateWorldMatrix(&matrix);
		Vector3 worldPosition{ sphere_.offset_ * matrix };

		// 引数で球を作る

		float distance{ (_center - worldPosition).Size() };
		float hitDistance{ sphere_.radius_ + _radius };

		// 距離が双方の球の半径よりも小さければ当たっている
		return (distance <= hitDistance);
	}
	else if (type_ == TYPE_CAPSULE)
	{
		// TODO: カプセルと球の当たり判定

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
