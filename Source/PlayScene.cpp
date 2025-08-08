#include "PlayScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Reticle.h"
#include "Camera.h"
#include "Network/PIIO.h"
#include "Background.h"
#include "SkySphere.h"
#include "TestScene/TestScene.h"

using namespace mtgb;
using Network::PIIO;

namespace
{
	static const mtnet::IPEndPoint LOCAL_IPEP{ "192.168.42.150", 60349 };
	static const mtnet::IPEndPoint SERVER_IPEP{ "192.168.42.62", 60349 };
}

PlayScene::PlayScene()
{
	tBox_ = new TextBox();
	ppiio_ = new PIIO{ LOCAL_IPEP };
}

PlayScene::~PlayScene()
{
}

void PlayScene::Initialize()
{
	Audio::Clear();

	Instantiate<Background>();

	//hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ -10, 0, -10 }));
	//SetCameraGameObject(Instantiate<Camera>());
	//WinCtxRes::Get<CameraResource>(WindowContext::First).SetCamera(Instantiate<Camera>(WindowContext::First));
	//WinCtxRes::Get<CameraResource>(WindowContext::Second).SetCamera(Instantiate<Camera>(WindowContext::Second));
	//hCamera1_ = Game::System<CameraSystem>().RegisterCamera()
	hCamera1_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ -10, 0, -10 }, WindowContext::First));
	hCamera2_ = RegisterCameraGameObject(Instantiate<Camera>(Vector3{ 10, 0, -10 }, WindowContext::Second));

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1_);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2_);
	//Instantiate<Player>(WindowContext::Second);
	Instantiate<SkySphere>();
	//Instantiate<Reticle>();
	Instantiate<Reticle>(WindowContext::First);
	Instantiate<Reticle>(WindowContext::Second);
	Instantiate<Player>(WindowContext::First);
	Instantiate<Enemy>(Vector3{ 0, 0, 10 });

	// 表示したいテキストを開始
	tBox_->SetTextSpeedSec(0.1f);
	tBox_->Show("hello this test");

	ppiio_->Start(SERVER_IPEP);
}

void PlayScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<TestScene>();
	}

	using LED_STATUS = Network::PIIO::LED_STATUS;
	//if (InputUtil::GetKeyDown(KeyCode::Escape))
	if (InputUtil::GetKeyDown(KeyCode::Escape, mtgb::WindowContext::Both))
	{
		Game::Exit();
	}

	if (InputUtil::GetKeyDown(KeyCode::Alpha1))
	{
		ppiio_->SendLED(LED_STATUS::LEDS_WARNNING);
	}
	if (InputUtil::GetKeyDown(KeyCode::Alpha2))
	{
		ppiio_->SendLED(LED_STATUS::LEDS_NORMAL);
	}
	if (InputUtil::GetKeyDown(KeyCode::Alpha3))
	{
		ppiio_->SendLED(LED_STATUS::LEDS_CLEAR);
	}

	if (InputUtil::GetKeyDown(KeyCode::P))
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

	if (InputUtil::GetKeyDown(KeyCode::Q))
	{
		val_ += 10;
		if (val_ > 100)
		{
			val_ = 100;
		}

		ppiio_->SendFan(static_cast<float>(val_) / 100.0f);
	}
	if (InputUtil::GetKeyDown(KeyCode::E))
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
