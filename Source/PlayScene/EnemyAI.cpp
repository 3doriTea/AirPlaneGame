#include "EnemyAI.h"

namespace
{
	// アクティブ範囲距離
	const float SLEEP_DISTANCE{ 300 };
	// 回避行動をとる距離
	const float AVOID_DISTANCE{ 50 };
	// 回避行動として注目する座標 プラス方向
	const float AVOID_LOOK_POS_ADD_Y{ 5 };
	// 回避行動として注目する座標 マイナス方向
	const float AVOID_LOOK_POS_SUB_Y{ 5 };
}

EnemyAI::EnemyAI()
{
	sMain_
		.OnStart(S_SLEEP,  [this]
		{
			out_.isActive = false;  // スリープ開始で非アクティブ化
		})
		.OnUpdate(S_SLEEP, [this]
		{
			// プレイヤーとの距離がアクティブ範囲内なら、索敵行動に遷移
			if (GetToPlayerDistance() <= SLEEP_DISTANCE)
			{
				sMain_.Change(S_SEARCH);
			}
		})
		.OnEnd(S_SLEEP, [this]
		{
			out_.isActive = true;  // スリープ終了でアクティブ化
		})

		.OnUpdate(S_SEARCH, [this]
		{
			LOGF("S_SEARCH\n");

			{ /* TODO:索敵行動 */
				out_.lookPosition = input_.pSelfTrans->GetWorldPosition() + input_.pSelfTrans->Forward() * 10 + input_.pSelfTrans->Right() * 1;
			}

			if (IsForwardToPlayerDir())
			{
				sMain_.Change(S_FIGHT);
			}
		})

		.OnStart(S_FIGHT, [this]
		{
			// 最初のStartを呼び出すために変更
			sFight_.Change(SF_LOOK_AT_PLAYER);

			LOGF("S_FIGHT\n");
		})
		.OnUpdate(S_FIGHT,  [this]
		{
			// 戦闘ステートに任せる
			sFight_.Update();
		});

	sFight_
		.OnStart(SF_LOOK_AT_PLAYER, [this] { out_.isFire = true; })
		.OnUpdate(SF_LOOK_AT_PLAYER, [this]
		{
			LOGF("SF_LOOK_AT_PLAYER\n");

			out_.lookPosition = input_.playerPos;

			// 衝突回避範囲内なら、回避行動に遷移
			if (GetToPlayerDistance() < AVOID_DISTANCE)
			{
				sFight_.Change(SF_AVOID);
			}
		})
		.OnStart(SF_LOOK_AT_PLAYER, [this] { out_.isFire = false; })

		.OnUpdate(SF_AVOID, [this]
		{
			LOGF("SF_AVOID\n");

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
			LOGF("SF_ROUND\n");

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

const bool EnemyAI::IsForwardToPlayerDir() const
{
	Vector3 vForward{ input_.pSelfTrans->Forward() };
	Vector3 vDir{ Vector3::Normalize(input_.playerPos - input_.pSelfTrans->GetWorldPosition()) };

	// プレイヤーが前方向に居るなら、戦闘行動に遷移
	float angle{ DirectX::XMVectorGetX(DirectX::XMVector3Dot(vForward, vDir)) };
	// cosが0未満なら前方向
	return angle > 0;
}
