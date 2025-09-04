#include "EventPopup.h"
//#include 

using namespace mtgb;

namespace
{
	const Vector2Int TEST_SCREEN_SIZE{ 1280, 720 };
	inline constexpr float ToRateY(const float _pixel)
	{
		return _pixel / TEST_SCREEN_SIZE.y;
	}
	inline constexpr float ToRateX(const float _pixel)
	{
		return _pixel / TEST_SCREEN_SIZE.x;
	}

	const int MESSAGE_BOX_POS_X_PX{ 40 };
	const Vector2Int MESSAGE_BOX_SIZE_PX{ 240, 80 };
	const float MOVE_SIZE_Y_PX_RATE{ ToRateY(120) };
	const float MOVE_BEGIN_Y_PX_RATE{ ToRateY(440) };
	const float SWING_TIME{ 0.1f };
	const float STOP_TIME{ 0.3f };
	// アニメーション全体の時間
	const float ANIM_TIME_MAX_SEC{ SWING_TIME + STOP_TIME + SWING_TIME };
}

EventPopup::EventPopup(const std::string& _message) : GameObject(GameObjectBuilder()
	.Build()),
	message_{ _message },
	timeLeft_{ 0.0f },
	MOVE_SIZE_Y_PX{ MOVE_SIZE_Y_PX_RATE * Game::System<Screen>().GetSize().y }
{
	movePointsTimeSec =
	{
		SWING_TIME,
		SWING_TIME + STOP_TIME,
		SWING_TIME + STOP_TIME + SWING_TIME,
	};
	int beginPosY{ static_cast<int>(MOVE_BEGIN_Y_PX_RATE * Game::System<Screen>().GetSize().y) };
	moveSizeY_ = static_cast<int>(MOVE_SIZE_Y_PX_RATE * Game::System<Screen>().GetSize().y);

	for (int i = 0; i < ANIM_MAX; i++)
	{
		movePointsYPx[i] = beginPosY + (moveSizeY_ * i);
	}
}

EventPopup::~EventPopup()
{
}

void EventPopup::Update()
{
	timeLeft_ += Time::DeltaTimeF();
	if (timeLeft_ > movePointsTimeSec[ANIM_C])
	{
		DestroyMe();
	}
}

void EventPopup::Draw() const
{
	int posY{};
	float anim{};
	const std::array<float, ANIM_MAX>& POINTS_TIME{ movePointsTimeSec };

	auto easeFunc
	{
		[](const float _v) -> float
		{
			return Ease::OutExpo(_v);
		}
	};

	if (timeLeft_ <= POINTS_TIME[ANIM_A])
	{
		anim = Mathf::Normalize(0.0f, POINTS_TIME[ANIM_A], timeLeft_);
		posY = movePointsYPx[ANIM_A] + moveSizeY_ * easeFunc(anim);
	}
	else if (timeLeft_ <= POINTS_TIME[ANIM_B])
	{
		anim = Mathf::Normalize(POINTS_TIME[ANIM_A], POINTS_TIME[ANIM_B], timeLeft_);
		posY = movePointsYPx[ANIM_B] + moveSizeY_ * easeFunc(anim);
	}
	else if (timeLeft_ <= POINTS_TIME[ANIM_C])
	{
		anim = Mathf::Normalize(POINTS_TIME[ANIM_B], POINTS_TIME[ANIM_C], timeLeft_);
		posY = movePointsYPx[ANIM_C] + moveSizeY_ * easeFunc(anim);
	}
	
	Draw::Box({ { MESSAGE_BOX_POS_X_PX, posY }, MESSAGE_BOX_SIZE_PX }, Color::BLUE);
	
	//Draw::ImmediateText(message_, MESSAGE_BOX_POS_X_PX, );
}
