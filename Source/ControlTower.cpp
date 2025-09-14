#include "ControlTower.h"
#include "TestScene/EnemyPlane.h"
#include "MTStringUtility.h"
#include <format>
ControlTower::ControlTower() : GameObject(GameObjectBuilder()
	.SetPosition({ 0,0,0 })
	.SetName("ControlTower")
	.Build())
	,detectionRadius_{30.0f}
	
{
	// 初期化 : nullptr
	pGunner_ = { nullptr,WindowContext::First };
	pPilot_ = { nullptr,WindowContext::Second };
}

ControlTower::~ControlTower()
{
}

void ControlTower::Update()
{
	Vector3 enemyPosGunner = Vector3::Zero();
	if (std::optional<Vector3> pos = DetectionEnemy(pGunner_.first, pGunner_.second); pos != std::nullopt )
	{
		enemyPosGunner = *pos;
	}
	Vector3 enemyPosPilot = Vector3::Zero();
	if (std::optional<Vector3> pos = DetectionEnemy(pPilot_.first, pPilot_.second); pos != std::nullopt)
	{
		enemyPosPilot = *pos;
	}
	
	MTImGui::Instance().DirectShow([enemyPosGunner,enemyPosPilot]()
		{
			ImGui::LabelText("enemyPosGunner", "(x,y):(%.3f,%.3f)",enemyPosGunner.x, enemyPosGunner.y);
			ImGui::LabelText("enemyPosPilot", "(x,y):(%.3f,%.3f)", enemyPosPilot.x, enemyPosPilot.y);
		}
	,"ControlTower",ShowType::Inspector);

}

void ControlTower::Draw() const
{
}

void ControlTower::SetGunner(EntityId _id, WindowContext _context)
{
	pGunner_.first = &Transform::Get(_id);
	pGunner_.second = _context;
}

void ControlTower::SetPilot(EntityId _id, WindowContext _context)
{
	pPilot_.first = &Transform::Get(_id);
	pPilot_.second = _context;
}

std::optional<Vector3> ControlTower::DetectionEnemy(Transform* _transform, WindowContext _context)
{
	// Enemyを取得
	std::vector<EnemyPlane*> enemies;
	FindGameObjects<EnemyPlane>(&enemies);

	// プレイヤーに対して戦闘を行う状態になっている敵のみにする
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(),
			[&](const EnemyPlane* _enemy) 
			{
				auto& mainState = _enemy->GetAI().GetMainState();
				auto& fightState = _enemy->GetAI().GetFightState();
				EnemyAI::MAIN_STATE currState = mainState.Current();
				// 戦闘状態でないならば trueにして破棄
				if (currState != EnemyAI::MAIN_STATE::S_FIGHT)
				{
					return true;
				}
				else
				{
					return false;
				}
				/*if (fightState.Current() == EnemyAI::FIGHT_STATE::SF_LOOK_AT_PLAYER)
				{

				}*/
			}),
		enemies.end()
		);

	// 敵がいないなら
	if (enemies.empty())
	{
		return std::nullopt;
	}

	// 先頭の敵のEntityIdを取得
	EntityId enemyId = enemies.front()->GetEntityId();

	// 敵が二体以上いるなら一番近くのを選ぶ
	if (enemies.size() >= 2)
	{
		// 一番近くの敵を取得
		auto itr =std::min_element(
			enemies.begin(),
			enemies.end(),
			[this,_transform](EnemyPlane* a, EnemyPlane* b)
			{
				float distanceA = (_transform->position - Transform::Get(a->GetEntityId()).position).Size();
				float distanceB = (_transform->position - Transform::Get(b->GetEntityId()).position).Size();
				return distanceA < distanceB;
			}
		);
		enemyId = (*itr)->GetEntityId();
	}

	// 方角を計算
	// プレイヤーの上ベクトル、右ベクトル
	Vector3 up = _transform->Up();
	Vector3 right = _transform->Right();
	Vector3 forward = _transform->Forward();
	
	Transform& enemy = Transform::Get(enemyId);
	Vector3 enemyScreenPos = Game::System<CameraSystem>().WorldToScreen(enemy.position, _context);
	
	return enemyScreenPos;
	// 敵のスクリーン座標を取得
	//Game::System<CameraSystem>().Get
	//for (const auto& enemy : enemies)
	//{
	//	Transform& enemyTransform = Transform::Get(enemy->GetEntityId());
	//	Vector3 toEnemy = Vector3::Normalize(enemyTransform.position - _transform->position);

	//	std::string str1= "", str2 = "",str3 = "";
	//	float horizontal = DirectX::XMVector3Dot((toEnemy), right).m128_f32[0];
	//	// 右
	//	if (horizontal == 0)
	//	{
	//		//str1 = "正面";
	//	}
	//	else if (horizontal > 0)
	//	{
	//		str1 = "右";
	//	}
	//	// 左
	//	else
	//	{
	//		str2 = "左";
	//	}
	//	
	//	float vertical = DirectX::XMVector3Dot(toEnemy, up).m128_f32[0];
	//	// 上
	//	if (vertical == 0)
	//	{
	//		//str2 = "正面";
	//	}
	//	else if (vertical > 0)
	//	{
	//		str2 = "上";
	//	}
	//	else
	//	{
	//		str2 = "下";
	//	}

	//	float upOrBack = DirectX::XMVector3Dot(toEnemy,forward ).m128_f32[0];
	//	// 正面
	//	if (upOrBack > 0)
	//	{
	//		str3 = "前方";
	//	}
	//	else if(upOrBack < 0)
	//	{
	//		str3 = "後ろ";
	//	}
	//	else if (upOrBack == 0)
	//	{
	//		str3 = "真横";
	//	}
	//	std::string str4 = "";
	//	
	//	LOGIMGUI("Camera%d,Enemy%lld:%s,%s,%s",_hCamera, enemy->GetEntityId(),str1.c_str(), str2.c_str(), str3.c_str());
	//	
	//	ret = std::format("{}:{},{},{}\n", enemy->GetEntityId(), str1, str2, str3);
	//}
	//return ret;
}
