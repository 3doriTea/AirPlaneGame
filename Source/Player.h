#pragma once
#include <mtgb.h>
#include "Camera.h"
//REFL_FORWARD_DECLARATION

class Player : public GameObject
{
	friend class PlayerProxy;
public:
	Player();
	Player(mtgb::WindowContext context);
	~Player();

	void Update() override;
	void Draw() const override;
	int test1;
private:
	PlayerProxy* proxy_;
	float test2;
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


