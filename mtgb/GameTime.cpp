#include "GameTime.h"
#include "IncludingWindows.h"
#include "Game.h"

#pragma comment(lib, "Winmm.lib")

mtgb::Time::Time() :
	current_{},
	previous_{}
{
}

mtgb::Time::~Time()
{
}

void mtgb::Time::Initialize()
{
	QueryPerformanceCounter(&previous_);
}

void mtgb::Time::Update()
{
	timeBeginPeriod(1);

	massert(QueryPerformanceCounter(&current_) == TRUE
		&& "QueryPerformanceCounter‚ÌŽæ“¾‚ÉŽ¸”s");

	if ((current_.QuadPart - previous_.QuadPart) * 60 >= SEC_TO_MICRO)
	{
		deltaTime_ = (current_.QuadPart - previous_.QuadPart) * MICRO_TO_SEC;
		previous_ = current_;
		Game::UpdateFrame();
	}

	timeEndPeriod(1);
}

double mtgb::Time::deltaTime_{};
const LONGLONG mtgb::Time::SEC_TO_MICRO{ 1000000 };
const double mtgb::Time::MICRO_TO_SEC{ 0.0000001 };
