#include "PlayerGunner.h"
#include "PlayerBullet.h"

using namespace mtgb;

PlayerGunner::PlayerGunner(const EntityId _plane) : GameObject(GameObjectBuilder()
	.Build()),
	pTransform_{ Component<Transform>() },
	angleX_{ 0.0f },
	angleY_{ 0.0f }
	,ImGuiShowable<PlayerGunner>("Gunner",Show::Inspector)
{
	pTransform_->SetParent(_plane);
}

PlayerGunner::~PlayerGunner()
{
}

void PlayerGunner::Update()
{
	constexpr float ANGLE_SPEED{ DirectX::XMConvertToRadians(100.f) };

	if (InputUtil::GetKey(KeyCode::W))
	{
		angleX_ -= ANGLE_SPEED * Time::DeltaTimeF();
		//pTransform_->Rotation(Vector3::Left() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::S))
	{
		angleX_ += ANGLE_SPEED * Time::DeltaTimeF();
		//pTransform_->Rotation(Vector3::Right() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::A))
	{
		angleY_ -= ANGLE_SPEED * Time::DeltaTimeF();
		//pTransform_->Rotation(Vector3::Down() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	if (InputUtil::GetKey(KeyCode::D))
	{
		angleY_ += ANGLE_SPEED * Time::DeltaTimeF();
		//pTransform_->Rotation(Vector3::Up() * ANGLE_SPEED * Time::DeltaTimeF());
	}
	pTransform_->rotate = Quaternion::Euler({ angleX_, angleY_, 0.0f });
	if (InputUtil::GetKeyDown(KeyCode::Space))
	{
		Instantiate<PlayerBullet>(pTransform_->position + Vector3::Down() * 1.0f, pTransform_->rotate);
		LOGIMGUI("Gunner:shoot");
	}
}

void PlayerGunner::Draw() const
{
}

void PlayerGunner::ShowImGui()
{
	ImGui::Text("%d,Gunner", entityId_);
}
