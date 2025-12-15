#pragma once
#include <mtgb.h>

class Cloud : public GameObject
{
public:
	Cloud();
	~Cloud();

	void Update() override;
	void Draw() const override;

private:
	ModelHandle hModel_;  // ‰_‚Ìƒ‚ƒfƒ‹
};
