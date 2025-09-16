#include "QuotaGauge.h"

using namespace mtgb;
namespace
{
	// UI設計時のキャンバスのサイズ
	/*Vector2F CANVAS_SIZE{ 1920.0f,1080.0f };*/

	const RectF QUOTA_GAUGE_RECT{ 450.0f,50.0f,1020.0f,40.0f };
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
	.Build())
{

	currentScoreBarRect_.point = QUOTA_GAUGE_RECT.point;
	toQuotaBarRect_.point = QUOTA_GAUGE_RECT.point;
	afterQuotaBarRect_.point = QUOTA_GAUGE_RECT.point;

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
	Vector2F screenSize{ Game::System<Screen>().GetSizeF() };
	Vector2F ratio = { screenSize.x / CANVAS_SIZE.x, screenSize.y / CANVAS_SIZE.y };
	
	RectF drawRect = GenDrawScreenFrom(QUOTA_GAUGE_RECT);
	/*{
		{ QUOTA_GAUGE_RECT.point.x, QUOTA_GAUGE_RECT.point.y },
		{ QUOTA_GAUGE_RECT.size.x, QUOTA_GAUGE_RECT.size.y }
	};*/
	// 仮の進捗状況、ノルマを割合で
	
	float quotaRatio = QUOTA / MAX_GAUGE_SCORE;
	float progressRatio = progress / MAX_GAUGE_SCORE;

	//Draw::Box((drawRect), AFTER_QUOTA_BAR_COLOR, { 0 });
	Draw::Box(drawRect, AFTER_QUOTA_BAR_COLOR, { 0 });

	drawRect.width = QUOTA_GAUGE_RECT.width * quotaRatio;

	//Draw::Box(GenDrawScreenFrom(drawRect), Color::RED,{ 1 });
	Draw::Box((drawRect), Color::RED,{ 1 });
	
	drawRect.width = QUOTA_GAUGE_RECT.width * progressRatio;

	//Draw::Box(GenDrawScreenFrom(drawRect), Color::GREEN,{2});
	Draw::Box((drawRect), Color::GREEN,{2});

}
