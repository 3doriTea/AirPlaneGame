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
	.Build()),
	currentPoint_{ 0 }
{
	hImageBackNormal_ = Image::Load("Image/QuotaGaugeBackNormal.png");
	hImageBackClear_ = Image::Load("Image/QuotaGaugeBackClear.png");
	hImageFill_ = Image::Load("Image/YellowScore 1.png");
	hImageAir_ = Image::Load("Image/GrayScore 1.png");
	hImageFillGood_ = Image::Load("Image/PurpleScore.png");
}

QuotaGauge::~QuotaGauge()
{
}

void QuotaGauge::Update()
{
	//progress = Game::System<ScoreManager>().GetScore();
}

void QuotaGauge::Draw() const
{
	// 背景描画
	// ノルマ達成しているなら
	if (IsQuotaClear())
	{
		Draw::Image(hImageBackClear_, GenDrawScreenFrom(DRAW_RECT_BACK), UI_PARAMS_BACK);
	}
	else
	{
		Draw::Image(hImageBackNormal_, GenDrawScreenFrom(DRAW_RECT_BACK), UI_PARAMS_BACK);
	}

	// セルを描画
	for (int i = 0; i < GAUGE_COUNT; i++)
	{
		RectF draw{ DRAW_RECT_CELL_FIRST };
		draw.x += DRAW_RECT_CELL_FIRST.width * i;
		int point{ i + 1 };
		ImageHandle hImage{};

		if (point <= currentPoint_)
		{
			if (point >= QUOTA_COUNT)
			{
				hImage = hImageFillGood_;
			}
			else
			{
				hImage = hImageFill_;
			}
		}
		else
		{
			hImage = hImageAir_;
		}
		Draw::Image(hImage, GenDrawScreenFrom(draw), UI_PARAMS_CELL);
	}

	// テキストの描画
	if (IsQuotaClear())
	{
		if (currentPoint_ == GAUGE_COUNT)
		{
			Draw::ImmediateText(
				"ゲージ超過中！",
				GenDrawScreenFrom(DRAW_RECT_TEXT),
				GenDrawScreenFontSize(DRAW_TEXT_FONT_SIZE),
				TextAlignment::center,
				UI_PARAMS_TEXT);
		}
		else
		{
			Draw::ImmediateText(
				std::format("+{}ポイント", currentPoint_ - QUOTA_COUNT),
				GenDrawScreenFrom(DRAW_RECT_TEXT),
				GenDrawScreenFontSize(DRAW_TEXT_FONT_SIZE),
				TextAlignment::center,
				UI_PARAMS_TEXT);
		}
	}
	else
	{
		Draw::ImmediateText(
			std::format("{}ポイント", QUOTA_COUNT - currentPoint_),
			GenDrawScreenFrom(DRAW_RECT_TEXT),
			GenDrawScreenFontSize(DRAW_TEXT_FONT_SIZE),
			TextAlignment::center,
			UI_PARAMS_TEXT);
	}
}

void QuotaGauge::AddPoint(const int _point)
{
	currentPoint_ += _point;

	// ゲージ限界値を超えているなら限界値に戻す
	if (currentPoint_ > GAUGE_COUNT)
	{
		currentPoint_ = GAUGE_COUNT;
	}
	// 0未満になっているなら0に戻す
	else if (currentPoint_ < 0)
	{
		currentPoint_ = 0;
	}
}

const int QuotaGauge::GetQuotaCount() const
{
	return QUOTA_COUNT;
}
