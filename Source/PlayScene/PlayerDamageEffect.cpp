#include "PlayerDamageEffect.h"

namespace
{
	const RectF DRAW_RECT_FIT{ 0.0f, 0.0f, 1920.0f, 1080.0f };
	const RectF DRAW_RECT_BEGIN{ -810.0f, -455.0f, 3539.33f, 1990.88f };
	const UIParams UI_PARAMS{ .depth = 40 };
	const float SHOW_TIME_SEC{ 2.0f };
}

PlayerDamageEffect::PlayerDamageEffect() : GameObject(GameObjectBuilder()
	.Build()),
	timeLeft_{ SHOW_TIME_SEC }
{
	hImage_ = Image::Load("Image/DamageEffect2.png");
}

PlayerDamageEffect::~PlayerDamageEffect()
{
}

void PlayerDamageEffect::Update()
{
	timeLeft_ -= Time::DeltaTimeF();
	if (timeLeft_ <= 0.0f)
	{
		DestroyMe();
	}
}

void PlayerDamageEffect::Draw() const
{
	float rate{ Ease::OutQuart(timeLeft_ / SHOW_TIME_SEC) };
	RectF draw
	{ 
		Mathf::Lerp(DRAW_RECT_BEGIN.x, DRAW_RECT_FIT.x, rate),
		Mathf::Lerp(DRAW_RECT_BEGIN.y, DRAW_RECT_FIT.y, rate),
		Mathf::Lerp(DRAW_RECT_BEGIN.width, DRAW_RECT_FIT.width, rate),
		Mathf::Lerp(DRAW_RECT_BEGIN.height, DRAW_RECT_FIT.height, rate),
	};
	Draw::Image(hImage_, GenDrawScreenFrom(draw), UI_PARAMS);
}
