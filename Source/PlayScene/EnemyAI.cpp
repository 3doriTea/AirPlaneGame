#include "EnemyAI.h"

namespace
{
	// アクティブ範囲距離
	const float SLEEP_DISTANCE{ 100 };
	// 回避行動をとる距離
	const float AVOID_DISTANCE{ 20 };
	// 回避行動として注目する座標 プラス方向
	const float AVOID_LOOK_POS_ADD_Y{ 20 };
	// 回避行動として注目する座標 マイナス方向
	const float AVOID_LOOK_POS_SUB_Y{ 20 };
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
			{ /* TODO:索敵行動 */
				out_.lookPosition = input_.playerPos;
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
			// プレイヤーより座標が上なら上方向に回避
			if (input_.pSelfTrans->GetWorldPosition().y > input_.playerPos.y)
			{
				out_.lookPosition = input_.playerPos + Vector3::Up() * AVOID_LOOK_POS_ADD_Y;
			}
			// プレイヤーより座標が下なら下方向に回避
			else
			{
				out_.lookPosition = input_.playerPos + Vector3::Down() * AVOID_LOOK_POS_ADD_Y;
			}

			sFight_.Change(SF_ROUND);
		})

		.OnStart(SF_ROUND, [this]{ out_.isRound = true; })
		.OnUpdate(SF_ROUND, [this]
		{
			// プレイヤーの前にいるなら回避行動に遷移
			if (IsForwardToPlayerDir())
			{
				sFight_.Change(SF_AVOID);
			}
			// プレイヤーの後ろにいるなら索敵に遷移
			else
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
	Vector3 vRight{ input_.pSelfTrans->Right() };
	Vector3 vDir{ input_.playerPos - input_.pSelfTrans->GetWorldPosition() };

	// プレイヤーが前方向に居るなら、戦闘行動に遷移
	float angle{ DirectX::XMVectorGetX(DirectX::XMVector3Dot(vRight, vDir)) };
	// cosが0未満なら前方向
	return angle < 0;
}
