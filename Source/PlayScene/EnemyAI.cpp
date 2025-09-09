#include "EnemyAI.h"

namespace
{
	// アクティブ範囲距離
	const float SLEEP_DISTANCE{ 100 };
	// 回避行動をとる距離
	const float AVOID_DISTANCE{ 20 };
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
			{ /* TODO:索敵行動 */ }

			if (IsForwardToPlayerDir())
			{
				sMain_.Change(S_FIGHT);
			}
		})
		.OnUpdate(S_FIGHT, [this]
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
		.OnUpdate(SF_LOOK_AT_PLAYER, [this]
		{
			//if ()
		});

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
