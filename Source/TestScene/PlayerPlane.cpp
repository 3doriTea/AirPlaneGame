 #include "PlayerPlane.h"

using namespace mtgb;

#define __X m128_f32[0]
#define __Y m128_f32[1]
#define __Z m128_f32[2]

PlayerPlane::PlayerPlane() : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.Build()),
	pTransform_{ Component<Transform>() },
	pRB_{ Component<RigidBody>() }
{
}

PlayerPlane::~PlayerPlane()
{
}

void PlayerPlane::Update()
{
	using DirectX::XMQuaternionRotationAxis;

	const float ROT_ANGLE{ Time::DeltaTimeF() };

	Quaternion curr{ pTransform_->rotate };

#if 1
	//if (InputUtil::GetKey(KeyCode::Up))
	{
		//pTransform_->Rotation(Vector3::Right(), -ROT_ANGLE);
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), -ROT_ANGLE * -InputUtil::GetAxis(Axis::Y, WindowContext::Second));
		//curr *= XMQuaternionRotationAxis(Vector3::Right(), -ROT_ANGLE * InputUtil::GetAxis(Axis::Y, WindowContext::Second));
	}
	//if (InputUtil::GetKey(KeyCode::Down))
	{
		//pTransform_->Rotation(Vector3::Right(), ROT_ANGLE);
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), ROT_ANGLE * InputUtil::GetAxis(Axis::Y, WindowContext::Second));
		//curr *= XMQuaternionRotationAxis(Vector3::Right(), ROT_ANGLE * -InputUtil::GetAxis(Axis::Y, WindowContext::Second));
	}
	//if (InputUtil::GetKey(KeyCode::Left))
	{
		//pTransform_->Rotation(Vector3::Up(), -ROT_ANGLE);
		curr *= XMQuaternionRotationAxis(pTransform_->Up(), -ROT_ANGLE * -InputUtil::GetAxis(Axis::X, WindowContext::Second));
		//curr *= XMQuaternionRotationAxis(Vector3::Up(), -ROT_ANGLE * -InputUtil::GetAxis(Axis::X, WindowContext::Second));
	}
	//if (InputUtil::GetKey(KeyCode::Right))
	{
		//pTransform_->Rotation(Vector3::Up(), ROT_ANGLE);
		curr *= XMQuaternionRotationAxis(pTransform_->Up(), ROT_ANGLE * InputUtil::GetAxis(Axis::X, WindowContext::Second));
		//curr *= XMQuaternionRotationAxis(Vector3::Up(), ROT_ANGLE * InputUtil::GetAxis(Axis::X, WindowContext::Second));
	}
#else
	if (InputUtil::GetKey(KeyCode::Up))
	{
		pTransform_->Rotation(Vector3::Right(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::Down))
	{
		pTransform_->Rotation(Vector3::Right(), ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::Left))
	{
		pTransform_->Rotation(Vector3::Up(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::Right))
	{
		pTransform_->Rotation(Vector3::Up(), ROT_ANGLE);
	}
#endif
	// 前方向、頭は上方向に
	Vector3 forward{ pTransform_->Forward() };
	//Vector3 forward = XMVector3Cross(pTransform_->Right(), Vector3::Down());
	//pTransform_->Right()
	Vector3 angleForward{ XMVector3Cross(pTransform_->Right(), Vector3::Down()) };

	LOGF("PLANEDIFF(%f, %f, %f)\n", forward - angleForward);

	curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);




	//curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);

	//curr = RemoveZRotation(curr);

	pTransform_->rotate = curr;
	pRB_->velocity_ = pTransform_->Forward() * 3.0f;

	Vector3 worldPos{ pTransform_->GetWorldPosition() };
	//LOGF("AA:Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);
	//LOGF("AXIS(%f, %f, %f) Ang:%f\n", axis.__X, axis.__Y, axis.__Z, localZAngle);
	//LOGF("Euler(%f, %f, %f) \n", axis.__X, axis.__Y, axis.__Z, localZAngle);

	/*Vector3 worldPos{ pTransform_->GetWorldPosition() };
	LOGF("Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);*/
}

void PlayerPlane::Draw() const
{
}

Quaternion PlayerPlane::RemoveZRotation(Quaternion _q) const
{
	using DirectX::XMMatrixRotationQuaternion;
	using DirectX::XMVector3Cross;
	using DirectX::XMQuaternionRotationMatrix;

	// 回転行列
	Matrix4x4 mRot{ XMMatrixRotationQuaternion(_q) };

	Vector3 vUp{ mRot.r[1] };  // 上方向のベクトル

	// z軸を0にした上方向ベクトル
	Vector3 projectedUp{ vUp.x, vUp.y, 0 };
	projectedUp.Normalize();

	// z軸+方向のベクトル
	Vector3 forward{ mRot.r[2] };

	Vector3 right{ XMVector3Cross(forward, projectedUp) };
	right.Normalize();

	Matrix4x4 mNewRot
	{
		DirectX::XMMATRIX
		{
			right,
			projectedUp,
			forward,
			{ 0, 0, 0, 1 }
		}
	};

	return XMQuaternionRotationMatrix(mNewRot);
}
