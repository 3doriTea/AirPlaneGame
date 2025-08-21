 #include "PlayerPlane.h"

using namespace mtgb;

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
	pTransform_->rotate = curr;
	pRB_->velocity_ = (pTransform_->Forward()) * 3.0f;

	/*Vector3 worldPos{ pTransform_->GetWorldPosition() };
	LOGF("Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);*/
}

void PlayerPlane::Draw() const
{
}
