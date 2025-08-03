#pragma once
#include <mtgb.h>
#include "AccessibleMacro.h"

//REFL_FORWARD_DECLARATION
REFL_FORWARD_DECLARATION()
class Player : public GameObject
{
	ACCESSIBLE_PRIVATE()
public:
	Player();
	Player(mtgb::WindowContext context);
	~Player();

	void Update() override;
	void Draw() const override;
	int test1;
	float test2;
private:
	std::string str;
	std::string name_;
	Transform* pTransform_;
	AudioPlayer* pAudioPlayer_;
	ImageHandle hImage_;
	AudioHandle hMnow_;
	OBJModelHandle hModel_;
	mtgb::WindowContext context_;
};
