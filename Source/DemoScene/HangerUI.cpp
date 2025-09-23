#include "HangerUI.h"

using namespace mtgb;

namespace
{
	const std::string IMAGE_FILE_HANGER_A { "RightHanger.png" };  // 格納庫UI右の画像
	const std::string IMAGE_FILE_DOOR_A   { "RightDoor.png" };    // 格納庫ドアUI右の画像
	const std::string IMAGE_FILE_HANGER_B { "LeftHanger.png" };   // 格納庫UI左の画像
	const std::string IMAGE_FILE_DOOR_B   { "LeftDoor.png" };     // 格納庫ドアUI左の画像

	// コントローラスティックの無効距離
	const float STICK_DEAD_ZONE{ 0.02f };

	const float MOVE_TIME_SEC{ 1.0f };  // ドアが完全に開くまでの時間

	const int DEPTH_HANGER{ 4 };  // 格納庫の画像デプス
	const int DEPTH_DOOR{ 3 };    // ドアの画像デプス
	const RectF DRAW_RECT_HANGER{ 0, 0, 1920, 1080 };
}

HangerUI::HangerUI(const mtgb::GameObjectLayer _layer) : GameObject(GameObjectBuilder()
	.SetLayerFlag(_layer)
	.Build()),
	moveTimeLeft_{ MOVE_TIME_SEC }
{
	if (_layer == GameObjectLayer::A)
	{
		hHangerImage_ = Image::Load(IMAGE_FILE_HANGER_A);
		hDoorImage_ =   Image::Load(IMAGE_FILE_DOOR_A);
	}
	else
	{
		hHangerImage_ = Image::Load(IMAGE_FILE_HANGER_B);
		hDoorImage_ = Image::Load(IMAGE_FILE_DOOR_B);
	}
}

HangerUI::~HangerUI()
{
}

void HangerUI::Update()
{
	bool addForce{ false };
	if (GetLayerFlag().Is(GameObjectLayer::A))
	{
		Vector2F axis = InputUtil::GetAxis(WindowContext::First);
		addForce = axis.x > STICK_DEAD_ZONE;
	}
	else
	{
		Vector2F axis = InputUtil::GetAxis(WindowContext::Second);
		addForce = axis.x < -STICK_DEAD_ZONE;
	}

	if (addForce)
	{
		moveTimeLeft_ -= Time::DeltaTimeF();
	}
	else
	{
		moveTimeLeft_ = MOVE_TIME_SEC;
	}
}

void HangerUI::Draw() const
{
	const GameObjectLayer LAYER{ GetLayerFlag().Is(GameObjectLayer::A) ? GameObjectLayer::A : GameObjectLayer::B };
	Draw::Image(hDoorImage_, drawRectDoor_, { .depth = DEPTH_DOOR, .layerFlag = LAYER });
	Draw::Image(hHangerImage_, DRAW_RECT_HANGER, { .depth = DEPTH_HANGER, .layerFlag = LAYER });
}
