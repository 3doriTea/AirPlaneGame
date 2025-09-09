#include "QuotaGauge.h"

using namespace mtgb;
namespace
{
	// UI設計時のキャンバスのサイズ
	/*Vector2F CANVAS_SIZE{ 1920.0f,1080.0f };*/
	RectF QUOTA_GAUGE_RECT{ 450.0f,50.0f,1020.0f,40.0f };
}

QuotaGauge::QuotaGauge() : GameObject(GameObjectBuilder()
	.SetName("QuotaGauge")
	.SetRotate(Quaternion::Identity())
	.Build())
{
	currentScoreBarImage_ = Image::Load("Image/greenScoreBar.png");
	toQuotaBarImage_ = Image::Load("Image/redScoreBar.png");
	afterQuotaBarImage_ = Image::Load("Image/purpleScoreBar.png");

	currentScoreBarRect_.point = QUOTA_GAUGE_RECT.point;
	toQuotaBarRect_.point = QUOTA_GAUGE_RECT.point;
	afterQuotaBarRect_.point = QUOTA_GAUGE_RECT.point;

}

QuotaGauge::~QuotaGauge()
{
}

void QuotaGauge::Update()
{

}

void QuotaGauge::Draw() const
{
	Vector2F screenSize{ Game::System<Screen>().GetSizeF() };
	Vector2F ratio = { screenSize.x / CANVAS_SIZE.x, screenSize.y / CANVAS_SIZE.y };
	
	RectF drawRect
	{
		{ QUOTA_GAUGE_RECT.point.x, QUOTA_GAUGE_RECT.point.y },
		{ QUOTA_GAUGE_RECT.size.x, QUOTA_GAUGE_RECT.size.y }
	};
	// 仮の進捗状況、ノルマを割合で
	float progress = 0.3f;
	float quota = 0.7f;

	Draw::Image(afterQuotaBarImage_, GenDrawScreenFrom(drawRect), {0});

	drawRect.width = QUOTA_GAUGE_RECT.width * quota;

	Draw::Image(toQuotaBarImage_, GenDrawScreenFrom(drawRect), { 1 });
	
	drawRect.width = QUOTA_GAUGE_RECT.width * progress;

	Draw::Image(currentScoreBarImage_, GenDrawScreenFrom(drawRect), {2});

}
