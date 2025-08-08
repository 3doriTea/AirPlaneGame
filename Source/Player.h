#pragma once
#include <mtgb.h>
#include "AccessibleMacro.h"
#include "Camera.h"

REFL_FORWARD_DECLARATION()

class Player : public GameObject
{
	//template<typename, typename, auto> friend struct refl::detail::member_descriptor;
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
	float radius_;
	Transform* pTransform_;
	AudioPlayer* pAudioPlayer_;
	ImageHandle hImage_;
	AudioHandle hMnow_;
	OBJModelHandle hModel_;
	mtgb::WindowContext context_;
	FBXModelHandle fModel_;
	Camera* pCamera_;
	RigidBody* pRigidbody_;
	float acceleration_;
};
