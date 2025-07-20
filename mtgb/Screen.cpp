#include "Screen.h"
#include <Windows.h>
#include <string>

namespace
{
	struct ProfileInt
	{
		const char* section;
		const char* param;
		int initValue;
		static const char* PATH;

		inline const int Get() const
		{
			return GetPrivateProfileInt(section, param, initValue, PATH);
		}
	};
	const char* ProfileInt::PATH{ "./setup.ini" };
}

mtgb::Screen::Screen() :
	width_{ 0 },
	height_{ 0 },
	fpsLimit_{ 0 }
{
}

mtgb::Screen::~Screen()
{
}

void mtgb::Screen::Initialize()
{
	// ƒXƒNƒŠ[ƒ“‚Ìî•ñ‚ğæ“¾
	width_    = ProfileInt{ .section = "SCREEN", .param = "Width",  .initValue = 800 }.Get();
	height_   = ProfileInt{ .section = "SCREEN", .param = "Height", .initValue = 600 }.Get();
	fpsLimit_ = ProfileInt{ .section = "GAME",   .param = "Fps",    .initValue = 60  }.Get();
}

void mtgb::Screen::Update()
{
}
