#pragma once
#include <mtgb.h>
#include <array>

class EventPopup : public mtgb::GameObject
{
	enum ANIM_KEY_POINT
	{
		ANIM_A,  // すうぃん
		ANIM_B,  // ぴたっ
		ANIM_C,  // んぃうす
		ANIM_MAX,  // スウィングアニメーションのキーポイント数
	};

public:
	EventPopup(const std::string& _message);
	~EventPopup();

	void Update() override;
	void Draw() const override;

private:
	const float MOVE_SIZE_Y_PX;
	std::string message_;
	float timeLeft_;
	int moveSizeY_;
	std::array<int, ANIM_MAX> movePointsYPx;
	std::array<float, ANIM_MAX> movePointsTimeSec;
};
