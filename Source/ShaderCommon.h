#pragma once
#include <mtgb.h>

enum struct GameShader
{
	WingEffect,
};

namespace game
{
	template<typename T>
	using IOriginalShader = mtgb::IOriginalShader<GameShader, T>;
}
