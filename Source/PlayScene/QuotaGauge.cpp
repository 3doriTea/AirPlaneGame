#include "QuotaGauge.h"

using namespace mtgb;
namespace
{
	// UI設計時のキャンバスのサイズ
	/*Vector2F CANVAS_SIZE{ 1920.0f,1080.0f };*/

	//const RectF QUOTA_GAUGE_RECT{ 450.0f,50.0f,1020.0f,40.0f };
	const RectF QUOTA_GAUGE_RECT{ 400.0f, 120.0f, 1120.0f, 80.0f };
	// 紫色
	const Color AFTER_QUOTA_BAR_COLOR = 0x800080;

	// ゲージが満タンになる値
	const uint32_t MAX_GAUGE_SCORE = 1000;

	// ノルマ
	float QUOTA = 700;

	// 進捗割合
	float progress = 0;
}

QuotaGauge::QuotaGauge() : GameObject(GameObjectBuilder()
	.SetName("QuotaGauge")
	.SetRotate(Quaternion::Identity())
	.Build()),
	SCORE_MAX{ Game::System<ScoreManager>().GetQuotaScore() }
{
	/*currentScoreBarRect_.point = QUOTA_GAUGE_RECT.point;
	toQuotaBarRect_.point = QUOTA_GAUGE_RECT.point;
	afterQuotaBarRect_.point = QUOTA_GAUGE_RECT.point;*/

	QUOTA = Game::System<ScoreManager>().GetQuotaScore();
}

QuotaGauge::~QuotaGauge()
{
}

void QuotaGauge::Update()
{
	progress = Game::System<ScoreManager>().GetScore();
}

void QuotaGauge::Draw() const
{
	//Vector2F screenSize{ Game::System<Screen>().GetSizeF() };
	//Vector2F ratio = { screenSize.x / CANVAS_SIZE.x, screenSize.y / CANVAS_SIZE.y };
	//
	//
	//RectF screenAdjustRect = GenDrawScreenFrom(QUOTA_GAUGE_RECT);
	//RectF drawRect = screenAdjustRect;
	///*{
	//	{ QUOTA_GAUGE_RECT.point.x, QUOTA_GAUGE_RECT.point.y },
	//	{ QUOTA_GAUGE_RECT.size.x, QUOTA_GAUGE_RECT.size.y }
	//};*/
	//// 仮の進捗状況、ノルマを割合で
	//
	//float quotaRatio = QUOTA / MAX_GAUGE_SCORE;
	//float progressRatio = progress / MAX_GAUGE_SCORE;

	//Draw::Box((drawRect), AFTER_QUOTA_BAR_COLOR, { 0 });

	auto drawFunc
	{
		[&](const RectF& _rect, const Color _color, const UIParams& _uIParams)
		{
			Draw::Box(GenDrawScreenFrom(_rect), _color, _uIParams);
		}
	};

	drawFunc(QUOTA_GAUGE_RECT, Color::WHITE, { 0 });
	//drawRect.width = screenAdjustRect.width * quotaRatio;

	//Draw::Box(GenDrawScreenFrom(drawRect), Color::RED,{ 1 });
	drawFunc(, Color::RED, { 1 });
	
	//drawRect.width = screenAdjustRect.width * progressRatio;

	//Draw::Box(GenDrawScreenFrom(drawRect), Color::GREEN,{2});
	drawFunc(, Color::GREEN, { 2 });

}
