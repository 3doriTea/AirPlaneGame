 #include "PlayerPlane.h"

using namespace mtgb;

namespace
{
	TextHandle testText_;
}

#define __X m128_f32[0]
#define __Y m128_f32[1]
#define __Z m128_f32[2]

PlayerPlane::PlayerPlane() : GameObject(GameObjectBuilder()
	.SetPosition({ 0, 0, 0 })
	.SetName("PlayerPlane")
	.Build()),
	pTransform_{ Component<Transform>() },
	pRB_{ Component<RigidBody>() },
	vVPlayer_{}
{
	testText_ = Text::Load("あいうえお", 72);
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
	// WindowContextを直接指定しない方いい
	Vector2F axis = InputUtil::GetAxis(WindowContext::Second);
	//if (axis.x != 0.0f || axis.y != 0.0f)
	{

		// 上
		if (axis.y > 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Right(), -ROT_ANGLE);
		}
		else if (axis.y < 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Right(), ROT_ANGLE);
		}
		// 右
		if (axis.x > 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Up(), ROT_ANGLE);
		}
		else if (axis.x < 0)
		{
			curr *= XMQuaternionRotationAxis(pTransform_->Up(), -ROT_ANGLE);
		}

		// 前方向、頭は上方向に
		Vector3 forward{ pTransform_->Forward() };
		curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);
		pTransform_->rotate = curr;
	}
#else
	if (InputUtil::GetKey(KeyCode::W))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Right(), ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Up(), -ROT_ANGLE);
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		curr *= XMQuaternionRotationAxis(pTransform_->Up(), ROT_ANGLE);
	}
	// 前方向、頭は上方向に
	Vector3 forward{ pTransform_->Forward() };
	curr = Quaternion::SLerp(curr, Quaternion::LookRotation(forward, Vector3::Up()), 0.01f);
	pTransform_->rotate = curr;
#endif

	if (InputUtil::GetKeyDown(KeyCode::F))
	{
		vVPlayer_.Play(u8"正面に敵が102体います");
	}


	//Vector3 forward = XMVector3Cross(pTransform_->Right(), Vector3::Down());
	//pTransform_->Right()
	//Vector3 angleForward{ XMVector3Cross(pTransform_->Right(), Vector3::Down()) };


	pRB_->velocity_ = pTransform_->Forward() * 3.0f;

	MTImGui::Instance().DirectShow([this]() {
		TypeRegistry::Instance().CallFunc(&pTransform_->position, "Position");
		TypeRegistry::Instance().CallFunc(&pTransform_->rotate, "Rotation");
		},"PlayerPlane", ShowType::Inspector);
	//LOGF("AA:Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);
	//LOGF("AXIS(%f, %f, %f) Ang:%f\n", axis.__X, axis.__Y, axis.__Z, localZAngle);
	//LOGF("Euler(%f, %f, %f) \n", axis.__X, axis.__Y, axis.__Z, localZAngle);
}

void PlayerPlane::Draw() const
{
	Draw::ImmediateText("aiueoあいうえお",{0,0});
	Draw::Text(testText_, { 0, 0 });
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
