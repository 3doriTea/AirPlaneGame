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
	pGunner_ = { nullptr,INVALID_HANDLE };
	pPilot_ = { nullptr,INVALID_HANDLE };
}

ControlTower::~ControlTower()
{
}

void ControlTower::Update()
{
	std::string gunnerText = "Gunner : "+ DetectionEnemy(pGunner_.first, pGunner_.second);
	std::string pilotText = "Pilot : " + DetectionEnemy(pPilot_.first,pPilot_.second);

	MTImGui::Instance().DirectShow([gunnerText,pilotText]()
		{
			ImGui::Text(MultiToUTF8(gunnerText).c_str());
			ImGui::Text(MultiToUTF8(pilotText).c_str());
		}
	,"ControlTower",ShowType::Inspector);
}

void ControlTower::Draw() const
{
}

void ControlTower::SetGunner(EntityId _id, CameraHandleInScene _hCamera)
{
	pGunner_.first = &Transform::Get(_id);
	pGunner_.second = _hCamera;
}

void ControlTower::SetPilot(EntityId _id, CameraHandleInScene _hCamera)
{
	pPilot_.first = &Transform::Get(_id);
	pPilot_.second = _hCamera;
}

std::string ControlTower::DetectionEnemy(Transform* _transform, CameraHandleInScene _hCamera)
{
	std::string ret="";

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
				if (mainState.Current() == EnemyAI::MAIN_STATE::S_FIGHT)
				{
					
				}
				/*if (fightState.Current() == EnemyAI::FIGHT_STATE::SF_LOOK_AT_PLAYER)
				{

				}*/
			}),
		enemies.end()
		);

	// 一番近くの敵を取得
	auto itr = std::min_element(
		enemies.begin(),
		enemies.end(),
		[this,_transform](const EnemyPlane& a, const EnemyPlane& b)
		{
			float distanceA = (_transform->position - Transform::Get(a.GetEntityId()).position).Size();
			float distanceB = (_transform->position - Transform::Get(b.GetEntityId()).position).Size();
			return distanceA < distanceB;
		}
	);

	// 方角を計算
	// プレイヤーの上ベクトル、右ベクトル
	Vector3 up = _transform->Up();
	Vector3 right = _transform->Right();
	Vector3 forward = _transform->Forward();
	Transform camera = Game::System<CameraSystem>().GetTransform(_hCamera);

	//Game::System<CameraSystem>().GetWorldToScreenData
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
