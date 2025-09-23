#include "Collider.h"
#include "Transform.h"
#include "DirectXMath.h"
#include "Matrix4x4.h"
#include "Draw.h"
#include "Debug.h"
#include <cfloat>
namespace
{
	mtgb::Matrix4x4 matrix{};
	DirectX::XMVECTORF32 unitVectorEpsilon{ FLT_EPSILON ,FLT_EPSILON ,FLT_EPSILON ,FLT_EPSILON };
	bool XMVECTORIsUnit(DirectX::FXMVECTOR _v)
	{
		DirectX::XMVECTOR difference = DirectX::XMVectorSubtract(DirectX::XMVector3Length(_v), DirectX::XMVectorSplatOne());
		return DirectX::XMVector4Less(DirectX::XMVectorAbs(difference), unitVectorEpsilon);	
	}
}

mtgb::Collider::Collider(EntityId _entityId) 
	: IComponent{ _entityId }
	, pTransform_{&Transform::Get(_entityId)}
	, isStatic_{false}
	, colliderTag_{ColliderTag::GAME_OBJECT}
{
}



mtgb::Collider::Collider(EntityId _entityId, ColliderTag _colliderTag)
	: IComponent{ _entityId }
	, colliderTag_{_colliderTag}
{

	switch (_colliderTag)
	{
		// 現在はゲームオブジェクトは動的、ステージは静的と断定しているが
		// 動的なステージなども追加されるかもしれないので注意
	case ColliderTag::GAME_OBJECT:
		isStatic_ = false;
		pTransform_ = &Transform::Get(_entityId);
		break;
	case ColliderTag::STAGE:
		isStatic_ = true;
		pTransform_ = nullptr;
		break;
	}
	
}

mtgb::Collider::~Collider()
{
}

void mtgb::Collider::UpdateBoundingData()
{
	switch (type_)
	{
	case TYPE_SPHERE:
		UpdateBoundingSphere();
		break;
	case TYPE_AABB:
		UpdateBoundingBox();
		break;
	case TYPE_CAPSULE:
		// TODO: カプセル初期化
		break;
	}
}

void mtgb::Collider::UpdateBoundingSphere()
{
	// Transformから現在の位置を取得してBoundingSphereを更新
	
	//pTransform_->GenerateWorldMatrix(&matrix);
		
	//computeSphere_.Center = pTransform_->position + computeSphere_.Center;
	//computeSphere_.Center = Vector3(computeSphere_.Center) * matrix;
	computeSphere_.Center = pTransform_->position;
}

void mtgb::Collider::UpdateBoundingBox()
{
	if (!isStatic_)
	{
		/*pTransform_->GenerateWorldMatrix(&matrix);
		computeBox_.Center = Vector3(computeBox_.Center) * matrix;*/
		computeBox_.Center = pTransform_->position;
	}
}

bool mtgb::Collider::IsHit(const Collider& _other) const
{
	using DirectX::XMVector3TransformCoord;

	// ステージ同士は接触しないものとする
	if (colliderTag_ == ColliderTag::STAGE && _other.colliderTag_ == ColliderTag::STAGE)
	{
		return false;
	}

	if (type_ == _other.type_)
	{
		if (type_ == TYPE_SPHERE)
		{
			//pTransform_->GenerateWorldMatrix(&matrix);
			//Vector3 worldPosition{ Vector3(computeSphere_.Center) * matrix };

			//_other.pTransform_->GenerateWorldMatrix(&matrix);
			//Vector3 otherWorldPosition{ (_other.computeSphere_.Center) * matrix };

			//float distance{ (otherWorldPosition - worldPosition).Size() };
			//float hitDistance{ computeSphere_.Radius + _other.computeSphere_.Radius};

			//// 距離が双方の球の半径よりも小さければ当たっている
			//return (distance <= hitDistance);
			return computeSphere_.Intersects(_other.computeSphere_);
		}
		else if (type_ == TYPE_AABB)
		{
			return computeBox_.Intersects(_other.computeBox_);
		}
	}
	else
	{
		if (type_ == TYPE_SPHERE)
		{	
			return computeSphere_.Intersects(_other.computeBox_);
		}
		else if (type_ == TYPE_AABB)
		{
			return computeBox_.Intersects(_other.computeSphere_);
		}
	}

	return false;
}

bool mtgb::Collider::IsHit(const Vector3& _origin, const Vector3& _dir, float* dist)
{
	/////
	// DirectXCollision.hのBoundingSphere::Intersectsをコピペした。
	// 何故かIntersectsに実引数が正常に渡されないから。
	/////
	using namespace DirectX;
	// まずBoundingSphereを最新状態に更新
	//const_cast<Collider*>(this)->UpdateBoundingData();
	
	// 方向ベクトルを正規化（元のベクトルは保持）
	Vector3 normalizedDir = Vector3::Normalize(_dir);
	
	XMVECTOR vNormalizeDir = XMLoadFloat3(&normalizedDir);
	if (!XMVECTORIsUnit(vNormalizeDir))
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
		Vector3 worldPosition{ Vector3(computeSphere_.Center) * matrix };

		// 引数で球を作る

		float distance{ (_center - worldPosition).Size() };
		float hitDistance{ computeSphere_.Radius + _radius };

		// 距離が双方の球の半径よりも小さければ当たっている
		return (distance <= hitDistance);
	}
	else if (type_ == TYPE_CAPSULE)
	{
		// TODO: カプセルと球の当たり判定

	}

	return false;
}

void mtgb::Collider::SetCenter(const Vector3& _center)
{
	if (type_ == TYPE_AABB)
	{
		computeBox_.Center = _center;
	}
	else
	{
		computeSphere_.Center = _center;
	}
}

void mtgb::Collider::SetExtents(const Vector3& _extents)
{
	computeBox_.Extents = _extents;
}

void mtgb::Collider::SetRadius(float _radius)
{
	computeSphere_.Radius = _radius;
}

void mtgb::Collider::Draw() const
{
	static Transform copyTransform{};

	//Draw::SetShaderOnce(ShaderType::Debug3D);

	switch (type_)
	{
	case mtgb::Collider::TYPE_SPHERE:
		copyTransform = *pTransform_;
		copyTransform.scale *= Vector3::One() * computeSphere_.Radius;
		//copyTransform.position += computeSphere_.Center;
		copyTransform.Compute();
		Draw::FBXModel(hSphereModel_, copyTransform, 0,ShaderType::Debug3D);
		break;
	case mtgb::Collider::TYPE_CAPSULE:
		break;
	case mtgb::Collider::TYPE_AABB:


		if (!isStatic_)
		{
			copyTransform = *pTransform_;
		}
		else
		{
			copyTransform.parent = INVALD_ENTITY;
		}
		
		// 軸並行なので回転はなし
		copyTransform.rotate = Quaternion{};

		if (isStatic_)
		{
			// 静的、transform不要なのでそのまま代入
			copyTransform.position = computeBox_.Center;
			copyTransform.scale = computeBox_.Extents * 2.0f;
		}
		else
		{
			// transformに合わせて位置、サイズを調整
			//copyTransform.position += computeBox_.Center;
			copyTransform.scale *= computeBox_.Extents * 2.0f;
		}
		
		copyTransform.Compute();
		Draw::FBXModel(hBoxModel_, copyTransform, 0, ShaderType::Debug3D);
		break;
	default:
		break;
	}
}

mtgb::FBXModelHandle mtgb::Collider::hSphereModel_{ mtgb::INVALID_HANDLE };
mtgb::FBXModelHandle mtgb::Collider::hBoxModel_{ mtgb::INVALID_HANDLE };
