#pragma once
#include <mtgb.h>

class SkipChecker : public GameObject
{
public:
	SkipChecker();
	~SkipChecker();

	void Update();
	void Draw();

private:
	float rate_;
};

