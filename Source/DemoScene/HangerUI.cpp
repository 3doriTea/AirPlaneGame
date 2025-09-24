#include "HangerUI.h"

using namespace mtgb;

namespace
{
	const std::string IMAGE_FILE_HANGER_A  { "Image/RightHanger.png" };  // 格納庫UI右の画像
	const std::string IMAGE_FILE_DOOR_A    { "Image/RightDoor.png" };    // 格納庫ドアUI右の画像
	const std::string IMAGE_FILE_HANGER_B  { "Image/LeftHanger.png" };   // 格納庫UI左の画像
	const std::string IMAGE_FILE_DOOR_B    { "Image/LeftDoor.png" };     // 格納庫ドアUI左の画像
	const std::string IMAGE_FILE_TEXT_KEEP { "Image/TextUIKeep.png" };   // 説明キープして！
	const std::string IMAGE_FILE_TEXT_READY{ "Image/TextUIReady.png" };  // 説明準備段階

	// コントローラスティックの無効距離
	const float STICK_DEAD_ZONE{ 0.02f };

	const float MOVE_TIME_SEC{ 1.0f };  // ドアが完全に開くまでの時間

	const int DEPTH_HINT{ 6 };    // ドアの画像デプス
	const int DEPTH_TEXT{ 5 };    // ドアの画像デプス
	const int DEPTH_HANGER{ 4 };  // 格納庫の画像デプス
	const int DEPTH_DOOR{ 3 };    // ドアの画像デプス
	const RectF DRAW_RECT_HANGER{ 0, 0, 1920, 1080 };
	const float DRWA_X_RIGHT_DOOR{ -580.0f };  // 右ドアの

	const float TO_MOVE_DOOR_X_RIGHT{ 1250 };
	const float TO_MOVE_DOOR_X_LEFT{ -1250 };

	const RectF DRAW_RECT_CON_ANIM_HINT{ 430.0f, 470.0f, 400.0f, 520.0f };
	const RectF DRAW_RECT_CON_XBOX_ANIM_HINT{ 1100.0f, 480, 660.0f, 520.0f };

	const std::string CON_FILE_NAME_XBOX{ "Image/Hint-YawLeft-Xbox.png" };
	const std::string CON_FILE_NAME_FLIG{ "Image/Hint-YawRight.png" };
}

HangerUI::HangerUI(const mtgb::GameObjectLayer _layer) : GameObject(GameObjectBuilder()
	.SetLayerFlag(_layer)
	.Build()),
	moveTimeLeft_{ MOVE_TIME_SEC },
	hTextImage_{ INVALID_HANDLE }
{
	hTextReadyImage_ = Image::Load(IMAGE_FILE_TEXT_READY);
	hTextKeepImage_ = Image::Load(IMAGE_FILE_TEXT_KEEP);
	
	if (_layer == GameObjectLayer::A)
	{
		hHangerImage_ = Image::Load(IMAGE_FILE_HANGER_A);
		hDoorImage_ =   Image::Load(IMAGE_FILE_DOOR_A);
		hConHintImage_ = Image::Load(CON_FILE_NAME_FLIG);
	}
	else
	{
		hHangerImage_ = Image::Load(IMAGE_FILE_HANGER_B);
		hDoorImage_ = Image::Load(IMAGE_FILE_DOOR_B);
		hConHintImage_ = Image::Load(CON_FILE_NAME_XBOX);
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

	hTextImage_ = hTextReadyImage_;
	if (addForce)
	{
		if (moveTimeLeft_ > 0.0f)
		{
			moveTimeLeft_ -= Time::DeltaTimeF();
		}
		else
		{
			// 力がかけられているかつ、もうタイマーは0以下
			hTextImage_ = hTextKeepImage_;
		}
	}
	else
	{
		moveTimeLeft_ = MOVE_TIME_SEC;
	}

	drawRectDoor_ = DRAW_RECT_HANGER;
	float rate{ Ease::OutBounce(1.0f - moveTimeLeft_ / MOVE_TIME_SEC) };
	
	drawRectText_ = DRAW_RECT_HANGER;
	if (GetLayerFlag().Is(GameObjectLayer::A))
	{
		drawRectDoor_.x = Mathf::Lerp(drawRectDoor_.x, TO_MOVE_DOOR_X_RIGHT, rate);
		drawRectText_.x = DRWA_X_RIGHT_DOOR;
	}
	else
	{
		drawRectDoor_.x = Mathf::Lerp(drawRectDoor_.x, TO_MOVE_DOOR_X_LEFT, rate);
	}
}

void HangerUI::Draw() const
{
	const GameObjectLayer LAYER{ GetLayerFlag().Is(GameObjectLayer::A) ? GameObjectLayer::A : GameObjectLayer::B };
	Draw::Image(hDoorImage_, GenDrawScreenFrom(drawRectDoor_), { .depth = DEPTH_DOOR, .layerFlag = LAYER });
	Draw::Image(hHangerImage_, GenDrawScreenFrom(DRAW_RECT_HANGER), { .depth = DEPTH_HANGER, .layerFlag = LAYER });
	
	Draw::Image(hTextImage_, GenDrawScreenFrom(drawRectText_), { .depth = DEPTH_TEXT, .layerFlag = LAYER });

	if (IsOpened() == false)
	{
		if (GetLayerFlag().Is(GameObjectLayer::A))
		{
			Draw::Image(hConHintImage_, GenDrawScreenFrom(DRAW_RECT_CON_ANIM_HINT), { .depth = DEPTH_TEXT, .layerFlag = LAYER });
		}
		else
		{
			Draw::Image(hConHintImage_, GenDrawScreenFrom(DRAW_RECT_CON_XBOX_ANIM_HINT), { .depth = DEPTH_TEXT, .layerFlag = LAYER });
		}
	}
}
