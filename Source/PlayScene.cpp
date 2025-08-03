#include "PlayScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "Network/PIIO.h"
#include "Background.h"
//#include "MTNet/Common.h"


using namespace mtgb;
using Network::PIIO;

namespace
{
	static const mtnet::IPEndPoint LOCAL_IPEP{ "192.168.42.150", 60349 };
	static const mtnet::IPEndPoint SERVER_IPEP{ "192.168.42.62", 60349 };
}

PlayScene::PlayScene()
{
	new Background();
	tBox_ = new TextBox();
	ppiio_ = new PIIO{ LOCAL_IPEP };
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
	hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ -10, 0, -10 }));
	hCamera2_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 10, 0, -10 }));

	Instantiate<Player>();
	//Instantiate<Enemy>();

	// 表示したいテキストを開始
	tBox_->SetTextSpeedSec(0.1f);
	tBox_->Show("hello this test");

	ppiio_->Start(SERVER_IPEP);
}

void PlayScene::Update()
{
	using LED_STATUS = Network::PIIO::LED_STATUS;
	if (InputData::GetKeyDown(KeyCode::Escape))
	{
		Game::Exit();
	}
	//LOGF("テスト");

	if (InputData::GetKeyDown(KeyCode::Alpha1))
	{
		ppiio_->SendLED(LED_STATUS::LEDS_WARNNING);
	}
	if (InputData::GetKeyDown(KeyCode::Alpha2))
	{
		ppiio_->SendLED(LED_STATUS::LEDS_NORMAL);
	}
	if (InputData::GetKeyDown(KeyCode::Alpha3))
	{
		ppiio_->SendLED(LED_STATUS::LEDS_CLEAR);
	}

	if (InputData::GetKeyDown(KeyCode::P))
	{
		CameraSystem& CAMERA_SYS{ Game::System<CameraSystem>() };
		if (CAMERA_SYS.GetDrawCamera() == hCamera1_)
		{
			CAMERA_SYS.SetDrawCamera(hCamera2_);
		}
		else
		{
			CAMERA_SYS.SetDrawCamera(hCamera1_);
		}
	}

	if (InputData::GetKeyDown(KeyCode::Q))
	{
		val_ += 10;
		if (val_ > 100)
		{
			val_ = 100;
		}

		ppiio_->SendFan(static_cast<float>(val_) / 100.0f);
	}
	if (InputData::GetKeyDown(KeyCode::E))
	{
		val_ -= 10;
		if (val_ < 0)
		{
			val_ = 0;
		}

		ppiio_->SendFan(static_cast<float>(val_) / 100.0f);
	}
}

void PlayScene::Draw() const
{
	tBox_->Draw();
}

void PlayScene::End()
{
}
