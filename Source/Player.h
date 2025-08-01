#pragma once
#include <mtgb.h>


class Player : public GameObject
{
public:
	Player();
	~Player();

	void Update() override;
	void Draw() const override;
private:
	std::string str;
	int test1;
	float test2;
	Transform* pTransform_;
	AudioPlayer* pAudioPlayer_;
	ImageHandle hImage_;
	AudioHandle hMnow_;
	OBJModelHandle hModel_;
	mtgb::WindowContext context_;
};
