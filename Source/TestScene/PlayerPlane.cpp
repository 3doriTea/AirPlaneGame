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

	if (InputUtil::GetKey(KeyCode::Up))
	{
		pTransform_->Rotation(Vector3::Left() * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::Down))
	{
		pTransform_->Rotation(Vector3::Right() * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::Left))
	{
		pTransform_->Rotation(Vector3::Down() * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::Right))
	{
		pTransform_->Rotation(Vector3::Up() * Time::DeltaTimeF());
	}
	pRB_->velocity_ = (pTransform_->Forward()) * 10.0f;

	/*Vector3 worldPos{ pTransform_->GetWorldPosition() };
	LOGF("Pos(%f, %f, %f)\n", worldPos.x, worldPos.y, worldPos.z);*/
}

void PlayerPlane::Draw() const
{
}
