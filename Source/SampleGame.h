#pragma once
#include <mtgb.h>

class SampleGame : public mtgb::Game
{
public:
	SampleGame();
	~SampleGame();

private:
	void SetupSystems(const RegisterSystemFuncHolder& _register) override;

	inline std::string_view GetVersion() const override
	{
		return "0.0.1";
	};
	inline std::string_view GetTitle() const override
	{
		return "‚½‚¾‚ÌƒTƒ“ƒvƒ‹ƒQ[ƒ€";
	};
};
