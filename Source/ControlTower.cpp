#include "ControlTower.h"
#include "TestScene/EnemyPlane.h"
ControlTower::ControlTower(const EntityId _plane) : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("ControlTower")
	.Build())
	,detectionRagius_{30.0f}
{
	pPlayerPlaneTransform_ = &Transform::Get(_plane);
}

ControlTower::~ControlTower()
{
}

void ControlTower::Update()
{
	DetectionEnemy();
}

void ControlTower::Draw() const
{
}

void ControlTower::DetectionEnemy()
{
	// Enemyを取得
	std::vector<EnemyPlane*> enemies;
	FindGameObjects<EnemyPlane>(&enemies);

	// lockOn状態のものだけにする
	std::remove_if(enemies.begin(), enemies.end(), 
		[&](const EnemyPlane* _enemy) {
			return !_enemy->LockOnTarget();
		});

	
	// 方角を計算
	// プレイヤーの上ベクトル、右ベクトル
	Vector3 up = pPlayerPlaneTransform_->Up();
	Vector3 right = pPlayerPlaneTransform_->Right();

	for (const auto& enemy : enemies)
	{
		Transform& enemyTransform = Transform::Get(enemy->GetEntityId());
		Vector3 toEnemy = Vector3::Normalize(enemyTransform.position - pPlayerPlaneTransform_->position);

		std::string str1= "", str2 = "",str3 = "";
		float horizontal = DirectX::XMVector3Dot((toEnemy), right).m128_f32[0];
		// 右
		if (horizontal > 0)
		{
			str1 = "右";
		}
		// 左
		else
		{
			str2 = "左";
		}
		
		float vertical = DirectX::XMVector3Dot(toEnemy, up).m128_f32[0];
		// 上
		if (vertical > 0)
		{
			str2 = "上";
		}
		else
		{
			str2 = "下";
		}

		bool isForward = DirectX::XMVector3Dot(toEnemy, pPlayerPlaneTransform_->Forward()).m128_f32[0] > 0;
		// 正面
		if (isForward)
		{
			str3 = "正面";
		}
		else
		{
			str3 = "後ろ";
		}

		LOGIMGUI("Enemy%lld:%s,%s,%s", enemy->GetEntityId(), str1, str2, str3);
	}

	MTImGui::Instance().DirectShow([this]()
		{

		},"ControlTower",ShowType::Inspector);
}
