#include "EventPopup.h"
//#include 

using namespace mtgb;

namespace
{
	const Vector2Int MESSAGE_BOX_SIZE_PX{ 240, 80 };
}

EventPopup::EventPopup(const std::string& _message) : GameObject(GameObjectBuilder()
	.Build()),
	message_{ _message },
	timeLeft_{ 0.0f }
{
}

EventPopup::~EventPopup()
{
}

void EventPopup::Update()
{
	timeLeft_ += Time::DeltaTimeF();
}

void EventPopup::Draw() const
{
	Draw::Box({}, Color::BLUE);
}
