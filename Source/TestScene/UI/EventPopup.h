#pragma once
#include <mtgb.h>

class EventPopup : public mtgb::GameObject
{
public:
	EventPopup(const std::string& _message);
	~EventPopup();

	void Update() override;
	void Draw() const override;

private:
	std::string message_;
	float timeLeft_;
};
