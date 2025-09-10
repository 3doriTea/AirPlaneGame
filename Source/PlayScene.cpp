#include "PlayScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Reticle.h"
#include "Camera.h"
#include "Network/PIIO.h"

#include "Background.h"
#include "SkySphere.h"
#include "TestScene/TestScene.h"
#include "ResultScene.h"
#include "TimeLimit.h"
#include "OverScene.h"
#include "PlayScene/EnemiesController.h"
#include "TestScene/PlayerPlane.h"
#include "TestScene/PlayerPilot.h"
#include "TestScene/PlayerGunner.h"

using namespace mtgb;
using Network::PIIO;

namespace
{
	static const mtnet::IPEndPoint LOCAL_IPEP{ "192.168.42.150", 60349 };
	static const mtnet::IPEndPoint SERVER_IPEP{ "192.168.42.62", 60349 };
}

PlayScene::PlayScene()
{
	ppiio_ = new PIIO{ LOCAL_IPEP };
}

PlayScene::~PlayScene()
{
	
	//delete ppiio_;
}

void PlayScene::Initialize()
{
	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Audio::Clear();

	Instantiate<Background>();

	PlayerPlane* pPlayerPlane{ Instantiate<PlayerPlane>() };
	EntityId eIdPlayer{ pPlayerPlane->GetEntityId() };

	PlayerPilot* pPilot{ Instantiate<PlayerPilot>(eIdPlayer) };
	CameraHandleInScene hCamera1 = RegisterCameraGameObject(pPilot);

	PlayerGunner* pGunner{ Instantiate<PlayerGunner>(eIdPlayer) };
	CameraHandleInScene hCamera2 = RegisterCameraGameObject(pGunner);


	EnemiesController* pEnemiesController{ Instantiate<EnemiesController>(pPlayerPlane->GetEntityId()) };
	pEnemiesController->Spawan({ 0, 0, 100 });
	//pEnemiesController->Spawan({ 0, 50, 300 });
	//pEnemiesController->Spawan({ 0, -50, 500 });
	//pEnemiesController->Spawan({ 0, 0, 1000 });

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);

	//Instantiate<Player>(WindowContext::Second);
	Instantiate<SkySphere>();
	//Instantiate<Reticle>();
	Instantiate<Reticle>(WindowContext::First);
	Instantiate<Reticle>(WindowContext::Second);
	//Instantiate<Player>(WindowContext::First);
	timeLimit_ = Instantiate<TimeLimit>();
	timeLimit_->RegisterOnEndTimerCallback([]() 
		{
			Game::System<SceneSystem>().Move<OverScene>();
		});
	// 表示したいテキストを開始

	// ラズパイと通信を開始
	ppiio_->Start(SERVER_IPEP);
}

void PlayScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<ResultScene>();
	}

	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		timeLimit_->StartTimer();
	}
	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		timeLimit_->PauseTimer();
	}
	if (InputUtil::GetKeyDown(KeyCode::I))
	{
		timeLimit_->ResumeTimer();
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
}

void PlayScene::End()
{
}
