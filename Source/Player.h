#pragma once
#include <mtgb.h>


class Player : public GameObject
{
	//friend class TypeRegistry;
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
	Transform* pTransform_;
	AudioPlayer* pAudioPlayer_;
	ImageHandle hImage_;
	AudioHandle hMnow_;
	OBJModelHandle hModel_;
	mtgb::WindowContext context_;
};
