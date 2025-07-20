#include "Color.h"

mtgb::Color::Color(
	const uint8_t _r,
	const uint8_t _g,
	const uint8_t _b,
	const uint8_t _alpha)
{
	component[static_cast<int32_t>(Component::Red)]   = _r;
	component[static_cast<int32_t>(Component::Green)] = _g;
	component[static_cast<int32_t>(Component::Blue)]  = _b;
	component[static_cast<int32_t>(Component::Alpha)] = _alpha;
}

mtgb::Color::Color(uint32_t _code)
{
	code = _code;
	// アルファ値は強制的に100%にする
	component[static_cast<int32_t>(Component::Alpha)] = UINT8_MAX;
}
