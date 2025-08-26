#include "Enemy.h"
#include "../mtgb/DirectX11Draw.h"

Transform* Enemy::pPlayerPlaneTransform_{ nullptr };

Enemy::Enemy(const Vector3& _position) : GameObject(GameObjectBuilder()
	.SetName("Enemy")
	.SetPosition(_position)
	.SetRotate(Quaternion::Identity())
	.SetScale(Vector3::One())
	.Build()),
	pTransform_{ Component<Transform>() },
	hp_{ 100 },
	radius_{ 30 },
	speed_{ 5.0f },
	lockOnAngle_{45.0f},
	lockOnDistance_{30.0f}
{
	//hModel_ = OBJ::Load("Model/OBJ/enemy.obj");
	hModel_ = Fbx::Load("Model/AirPlene.fbx");
	//hModel_ = Fbx::Load("Assets/Model/gCube.fbx");
	massert(hModel_ >= 0 && "敵モデルの読み込みに失敗");

	hModelCollider_ = Fbx::Load("Model/SphereCollider.fbx");
	massert(hModelCollider_ >= 0 && "当たり判定モデルの読み込みに失敗");

	pTransform_->position.z = 5.0f;
	// プレイヤーの機体のTransformを取得
	pPlayerPlaneTransform_ = FindGameObject("PlayerPlane")->Component<Transform>();
}

Enemy::~Enemy()
{
}

void Enemy::Update()
{
}

void Enemy::Draw() const
{
	//Draw::OBJModel(hModel_, pTransform_);
	//Draw::FBXModel(hModel_, *pTransform_, 0);

	Draw::SetShaderOnce(ShaderType::Debug3D);
	Draw::FBXModel(hModelCollider_, *pTransform_, 0);
}

void Enemy::Search()
{
	Vector3 forward = pTransform_->Forward();
	Vector3 toPlayer = pPlayerPlaneTransform_->position - pTransform_->position;
	float distance = toPlayer.Size();

	// 内積
	float cosTheta = DirectX::XMVector3Dot(forward, Vector3::Normalize(toPlayer)).m128_f32[0];

	// ロックオンする、視野に入っていると判定する角度のラジアン
	float lockOnAngleRadian = DirectX::XMConvertToRadians(lockOnAngle_);

	if (cosTheta > lockOnAngleRadian)
	{
		LOGIMGUI("Enemy:%lld Lock On Player!!!", entityId_);
	}
	
	
}


