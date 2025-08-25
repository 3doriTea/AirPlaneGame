#include "EnemyPlane.h"

using namespace mtgb;

namespace
{
	TextHandle hText;
	const int HIT_DAMAGE{ 10 };
	const float BROKEN_DOWN_SPEED{ 30.0f };
}

EnemyPlane::EnemyPlane(
	const Vector3& _worldPosition,
	const EntityId _playerPlane) : GameObject(GameObjectBuilder()
	.SetName("Enemy")
	.SetPosition(_worldPosition)
	.Build()),
	pRB_{ Component<RigidBody>() },
	pTransform_{ Component<Transform>() },
	pCollider_{ Component<Collider>() },
	pTarget_{ &Transform::Get(_playerPlane) },
	speed_{ 10.0f },
	health_{}
{
	pCollider_->type_ = Collider::TYPE_SPHERE;
	pCollider_->sphere_.offset_ = Vector3::Zero();
	pCollider_->sphere_.radius_ = 1.0f;
	
	
	//hText = Text::Load("apple", 72);
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
	massert(hModel_ >= 0 && "敵飛行機モデル読み込みに失敗");

	pRB_->OnCollisionEnter([this](EntityId _targetId)
		{
			LOGF("Id:%d(%s)と衝突した！ by %d(%s)\n", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());
			LOGIMGUI("Id:%d(%s)と衝突した！ by %d(%s)", _targetId, FindGameObject(_targetId)->GetName().c_str(), entityId_, GetName().c_str());
			GameObject* pTarget{ FindGameObject(_targetId) };

			massert(pTarget != nullptr && "当たったが、相手のゲームオブジェクトが見つからなかった");
			if (pTarget->GetName() == "PlayerBullet")
			{
				pTarget->DestroyMe();
				health_.Damage(HIT_DAMAGE);
				if (health_.IsDead())
				{
					broken_ = true;  // 体力的に死んでいるなら飛行機を壊す
				}
				//DestroyMe();
			}
		});
}

EnemyPlane::~EnemyPlane()
{
}

void EnemyPlane::Update()
{
	if (broken_)
	{
		Quaternion lookQuaternion{ Quaternion::FromToRotation(pTransform_->Forward(), Vector3::Down())};
		pTransform_->rotate = Quaternion::SLerp(pTransform_->rotate, lookQuaternion, Time::DeltaTimeF());
		pRB_->velocity_ = pTransform_->Forward() * BROKEN_DOWN_SPEED;

		return;
	}
	//Vector3 diffDir{ pTarget_->position - pTransform_->position };
	//DirectX::XMQuaternionBaryCentric
	/*Vector3 diffDir{ pTarget_->position - pTransform_->position };
	Quaternion lookQuaternion{ Quaternion::LookRotation(diffDir, pTransform_->Up()) };*/
	

	MTImGui::Instance().TypedShow(pTransform_, "EnemyPlane:" + std::to_string(entityId_));
	MTImGui::Instance().DrawRay(pTransform_->position, pTransform_->Forward() * speed_, 2.0f);
}

void EnemyPlane::Draw() const
{
	//Draw::SetShaderOnce(ShaderType::Unlit3D);
	Draw::FBXModel(hModel_, *pTransform_, 0);
	pCollider_->Draw();
	Vector2Int pos = InputUtil::GetMousePosition();
	
	/*Draw::ChangeTextAlignment(TextAlignment::center);
	Draw::Text(hText, 0, 0);
	Draw::ImmediateText("Banana", 0, 0, 72, TextAlignment::topLeft);*/
	//Draw::ImmediateText("hello world",0,0);
	//Game::System<ColliderCP>().TestDraw();
	
}
