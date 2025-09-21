#include "QuotaGauge.h"
#include <format>

using namespace mtgb;
namespace
{
	// UI設計時のキャンバスのサイズ
	/*Vector2F CANVAS_SIZE{ 1920.0f,1080.0f };*/

	const UIParams UI_PARAMS_BACK{ .depth = 5 };
	const UIParams UI_PARAMS_CELL{ .depth = 6 };
	const UIParams UI_PARAMS_TEXT{ .depth = 7 };

	const RectF DRAW_RECT_CELL_FIRST{ 450, 90, 20, 40 };  // 一番左端のセル描画範囲
	const RectF DRAW_RECT_BACK{ 440, 40, 1040, 100 };  // ノルマバーの背景描画範囲
	const RectF DRAW_RECT_TEXT{ 1220, 48, 168, 22 };  // 表示テキストの描画範囲

	const int DRAW_TEXT_FONT_SIZE{ 18 };  // 表示テキストのフォントサイズ

	int QUOTA_COUNT{ 30 };  // ノルマ数
	int GAUGE_COUNT{ 50 };  // ゲージ数
}

QuotaGauge::QuotaGauge() : GameObject(GameObjectBuilder()
	.SetName("QuotaGauge")
	.SetRotate(Quaternion::Identity())
	.Build())
{
	hImageBackNormal_ = Image::Load("Image/QuotaGaugeBackNormal.png");
	hImageFill_ = Image::Load("Image/YellowScore 1.png");
	hImageAir_ = Image::Load("Image/GrayScore 1.png");
}

QuotaGauge::~QuotaGauge()
{
}

void QuotaGauge::Update()
{
	//progress = Game::System<ScoreManager>().GetScore();
	currentPoint_ = QUOTA_COUNT;
}

void QuotaGauge::Draw() const
{
	// 背景描画
	Draw::Image(hImageBackNormal_, GenDrawScreenFrom(DRAW_RECT_BACK), UI_PARAMS_BACK);

	// セルを描画
	for (int i = 0; i < GAUGE_COUNT; i++)
	{
		RectF draw{ DRAW_RECT_CELL_FIRST };
		draw.x += DRAW_RECT_CELL_FIRST.width * i;
		Draw::Image(
			(i <= currentPoint_) ? hImageFill_ : hImageAir_,
			GenDrawScreenFrom(draw),
			UI_PARAMS_CELL);
	}

	// テキストの描画
	Draw::ImmediateText(
		std::format("{}ポイント", currentPoint_),
		GenDrawScreenFrom(DRAW_RECT_TEXT),
		GenDrawScreenFontSize(DRAW_TEXT_FONT_SIZE),
		TextAlignment::center,
		UI_PARAMS_TEXT);
}
