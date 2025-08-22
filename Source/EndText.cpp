#include "EndText.h"
#include "Draw.h"

EndText::EndText() : GameObject(GameObjectBuilder()
	.SetName("text")
	.SetPosition(Vector3(0, 0, 0))
	.SetRotate(Quaternion::Identity())
	.SetScale({ 1, 1, 1 })
	.Build())
{

}

EndText::~EndText()
{
}

void EndText::Update()
{
}

void EndText::Draw() const
{
	Draw::ImmediateText("tekitaugogo", 0, 0, 100);
}
