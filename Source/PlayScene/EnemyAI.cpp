#include "EnemyAI.h"
#include "MTImGui.h"
namespace
{
	// アクティブ範囲距離
	const float SLEEP_DISTANCE{ 200 };
	// 回避行動をとる距離
	const float AVOID_DISTANCE{ 50 };
	// 回避行動として注目する座標 プラス方向
	const float AVOID_LOOK_POS_ADD_Y{ 5 };
	// 回避行動として注目する座標 マイナス方向
	const float AVOID_LOOK_POS_SUB_Y{ 5 };
	// 見つかる視野角
	const float FOUND_FOV{ 60.0f };
}

EnemyAI::EnemyAI()
{
	sMain_
		.OnAnyUpdate([this] 
			{
				if (input_.isBroken && sMain_.Current() != MAIN_STATE::S_CRASH)
				{
					sMain_.Change(MAIN_STATE::S_CRASH);
				}
			})
		.OnStart(S_SLEEP, [this]
			{
				out_.isActive = false;  // スリープ開始で非アクティブ化
			})
		.OnUpdate(S_SLEEP, [this]
			{
				float distance{ GetToPlayerDistance() };
				// プレイヤーとの距離がアクティブ範囲内なら、索敵行動に遷移
				if (distance <= SLEEP_DISTANCE && distance > FLT_EPSILON)
				{
					LOGF("アクティブ化 距離%f", GetToPlayerDistance());
					sMain_.Change(S_SEARCH);
				}
			})
		.OnEnd(S_SLEEP, [this]
			{
				out_.isActive = true;  // スリープ終了でアクティブ化
			})

		.OnUpdate(S_SEARCH, [this]
			{
				{ /* TODO:索敵行動 */
					Vector3 pPos{ input_.playerPos };
					Vector3 sPos{ input_.pSelfTrans->GetWorldPosition() };
					out_.lookPosition = input_.pSelfTrans->GetWorldPosition() + input_.pSelfTrans->Forward() * 10 + input_.pSelfTrans->Right() * 2;
					if (std::fabsf(pPos.y - sPos.y) < 10.0f)
					{

					}
					else if (pPos.y < sPos.y)
					{
						out_.lookPosition += Vector3::Down() * 10.0f;
					}
					else
					{
						out_.lookPosition += Vector3::Up() * 10.0f;
					}

					out_.lookPosition += (pPos - sPos).Normalize() * 1.1f;
				}

				// アクティブ範囲外まで逃げたならプレイヤー方向に向く
				if (GetToPlayerDistance() > SLEEP_DISTANCE)
				{
					//sMain_.Change(S_SLEEP);
					sFight_.Change(SF_LOOK_AT_PLAYER);
				}
				// プレイヤーが視野に入ったら攻撃行動
				else if (IsForwardToPlayerDir())
				{
					sMain_.Change(S_FIGHT);
				}
			})

		.OnStart(S_FIGHT, [this]
			{
				// 最初のStartを呼び出すために変更
				sFight_.Change(SF_LOOK_AT_PLAYER);

				// 現在の状態、Idを通知
				Game::System<EventManager>().GetEvent<EventData>().Invoke(
					{
						.mainState = sMain_.Current(),
						.fightState = sFight_.Current(),
						.id = input_.pSelfTrans->GetEntityId()
					});
			})
		.OnUpdate(S_FIGHT, [this]
			{
				// 戦闘ステートに任せる
				sFight_.Update();
			})
		// 墜落行動
		.OnStart(S_CRASH, [this]
			{
				out_ =
				{
				.isActive = false,
				.isFire = false,
				.isRound = false,
				.isAvoiding = false,
				.lookPosition = Vector3::Zero()
				};
			});

	sFight_
		.OnStart(SF_LOOK_AT_PLAYER, [this] { out_.isFire = true; })
		.OnUpdate(SF_LOOK_AT_PLAYER, [this]
		{
			out_.lookPosition = input_.playerPos;

			// 衝突回避範囲内なら、回避行動に遷移
			if (GetToPlayerDistance() < AVOID_DISTANCE)
			{
				out_.isAvoiding = true;
				sFight_.Change(SF_AVOID);
			}
		})
		.OnEnd(SF_LOOK_AT_PLAYER, [this] { out_.isFire = false; })

		.OnStart(SF_AVOID, [this]
		{
		})
		.OnUpdate(SF_AVOID, [this]
		{
			out_.isAvoiding = false;
			out_.lookPosition = input_.pSelfTrans->GetWorldPosition() + input_.pSelfTrans->Forward() * 10;
			// プレイヤーより座標が上なら上方向に回避
			if (input_.pSelfTrans->GetWorldPosition().y > input_.playerPos.y)
			{
				out_.lookPosition += Vector3::Up() * AVOID_LOOK_POS_ADD_Y;
			}
			// プレイヤーより座標が下なら下方向に回避
			else
			{
				out_.lookPosition += Vector3::Down() * AVOID_LOOK_POS_ADD_Y;
			}

			sFight_.Change(SF_ROUND);
		})

		.OnStart(SF_ROUND, [this]{ out_.isRound = true; })
		.OnUpdate(SF_ROUND, [this]
		{
			out_.lookPosition = input_.pSelfTrans->GetWorldPosition() + input_.pSelfTrans->Forward() * 10;
			// プレイヤーの前にいるなら回避行動に遷移
			if (IsForwardToPlayerDir())
			{
				sFight_.Change(SF_AVOID);
			}
			// プレイヤーの後ろにいる && 回避行動圏外なら索敵に遷移
			else if (GetToPlayerDistance() >= AVOID_DISTANCE)
			{
				sMain_.Change(S_SEARCH);
			}
		})
		.OnEnd(SF_ROUND, [this]{ out_.isRound = false; })
;
	// 最初のStartを呼び出すために変更する
	sMain_.Change(S_SLEEP);
}

const float EnemyAI::GetToPlayerDistance() const
{
	return (input_.playerPos - input_.pSelfTrans->GetWorldPosition()).Size();
}

void EnemyAI::Update()
{
	MAIN_STATE prevMainState = sMain_.Current();
	FIGHT_STATE prevFightState = sFight_.Current();

	sMain_.Update(); 

	MAIN_STATE currMainState = sMain_.Current();
	FIGHT_STATE currFightState = sFight_.Current();

	if (prevMainState != currMainState || prevFightState != currFightState)
	{
		Game::System<EventManager>().GetEvent<EventData>().Invoke(
			{
				.mainState = currMainState,
				.fightState = currFightState,
				.id = input_.pSelfTrans->GetEntityId()
			});
	}

	std::string mainStateStr;
	switch (sMain_.Current())
	{
	case MAIN_STATE::S_FIGHT:
		mainStateStr = "S_FIGHT";
		break;
	case MAIN_STATE::S_SEARCH:
		mainStateStr = "S_SEARCH";
		break;
	case MAIN_STATE::S_SLEEP:
		mainStateStr = "S_SLEEP";
		break;
	case MAIN_STATE::S_CRASH:
		mainStateStr = "S_CRASH";
		break;
	default :
		mainStateStr = "Unknown";
		break;
	}

	std::string fightStateStr;
	switch (sFight_.Current())
	{
	case FIGHT_STATE::SF_AVOID:
		fightStateStr = "SF_AVOID";
		break;
	case FIGHT_STATE::SF_LOOK_AT_PLAYER:
		fightStateStr = "SF_LOOK_AT_PLAYER";
		break;
	case FIGHT_STATE::SF_ROUND:
		fightStateStr = "SF_ROUND";
		break;
	default:
		fightStateStr = "Unknown";
		break;
	}
	EntityId id = input_.pSelfTrans->GetEntityId();
	const std::string name = "EnemyAI:" + std::to_string(id);
	/*MTImGui::Instance().DirectShow([mainStateStr,fightStateStr]() {		
		ImGui::Text("MAIN_STATE : %s",mainStateStr.c_str());
		ImGui::Text("FIGHT_STATE : %s",fightStateStr.c_str());
		},name, ShowType::Inspector);*/
}
const bool EnemyAI::IsForwardToPlayerDir() const
{
	Vector3 vForward{ input_.pSelfTrans->Forward() };
	Vector3 vDir{ Vector3::Normalize(input_.playerPos - input_.pSelfTrans->GetWorldPosition()) };

	// プレイヤーが前方向に居るなら、戦闘行動に遷移
	float angle{ DirectX::XMVectorGetX(DirectX::XMVector3Dot(vForward, vDir)) };
	// cosが0未満なら前方向
	return angle > std::cosf(DirectX::XMConvertToRadians(FOUND_FOV));
}
