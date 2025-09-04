#pragma once
#include <mtgb.h>
#include <functional>
#include <queue>

//・制限時間の表示
//・制限時間に達したらノルマ確認
//・オーバーシーンへ(クリアor失敗)
//・クリア、ゲームオーバーの演出(仮のテキスト)

class TimeLimit : public mtgb::GameObject
{
public:
	TimeLimit();
	TimeLimit(float _time);
	~TimeLimit();

	void Update() override;
	void Draw() const override;
	/// <summary>
	/// タイマーのカウントダウンを始める
	/// </summary>
	void StartTimer();

	/// <summary>
	/// 制限時間に達した時に呼ばれるコールバックをセット
	/// </summary>
	void RegisterOnEndTimerCallback(std::function<void()> _callback);

	/// <summary>
	/// タイマーのカウントダウンを一時停止
	/// </summary>
	void PauseTimer();

	/// <summary>
	/// タイマーのカウントダウンを再開
	/// </summary>
	void ResumeTimer();

	/// <summary>
	/// 残り時間、フラグ、コールバック等すべてリセット
	/// </summary>
	void Reset();
	static constexpr float PLAY_SCENE_TIMER = 18.0f; // プレイシーンの制限時間(仮)
private:
	std::queue<std::function<void()>> onEndTimerCallbacks_;
	float remainingTime_; // 残り時間
	bool isStartTimer_; // カウントダウンが始まったか
	bool isResumeTimer_; // カウントダウンが一時停止しているか
};