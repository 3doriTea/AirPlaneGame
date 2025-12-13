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
#include "PlayScene/QuotaGauge.h"
#include "PlayScene/AutoPilotPlay.h"

#include "../Source/ControlTower.h"
#include "../Source/Stage.h"
#include <istream>
using namespace mtgb;
using Network::PIIO;

namespace
{
	static const mtnet::IPEndPoint LOCAL_IPEP{ "192.168.1.10", 60349 };
	static const mtnet::IPEndPoint SERVER_IPEP{ "192.168.1.20", 60349 };

	const float LED_TIME_CLEAR{ 2.0f };
	const float LED_TIME_WARNNING{ 2.0f };
}

PlayScene::PlayScene() :
	pAutoPilot_{ new AutoPilotPlay{} }
{
	ppiio_ = new PIIO{ LOCAL_IPEP };
}

PlayScene::~PlayScene()
{
	SAFE_DELETE(pAutoPilot_);
	/*ppiio_->SendLED(PIIO::LED_STATUS::LEDS_SLEEP);
	try
	{
		delete ppiio_;
	}
	catch(int)
	{

	}*/
}

void PlayScene::Initialize()
{
	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Audio::Clear();

	// スコアリセット
	Game::System<ScoreManager>().ResetScore();

	Instantiate<Background>();

	PlayerPlane* pPlayerPlane{ Instantiate<PlayerPlane>(pAutoPilot_) };
	EntityId eIdPlayer{ pPlayerPlane->GetEntityId() };
	pAutoPilot_->SetTransform(&Transform::Get(eIdPlayer));

	PlayerPilot* pPilot{ Instantiate<PlayerPilot>(eIdPlayer) };
	CameraHandleInScene hCamera1 = RegisterCameraGameObject(pPilot);

	PlayerGunner* pGunner{ Instantiate<PlayerGunner>(eIdPlayer) };
	CameraHandleInScene hCamera2 = RegisterCameraGameObject(pGunner);

	ControlTower* pControlTower{ Instantiate<ControlTower>() };
	pControlTower->SetControlTarget(pGunner->GetEntityId(), WindowContext::Second);
	pControlTower->SetControlTarget(pPilot->GetEntityId(), WindowContext::First);

	std::ifstream input("skyCombatStageDataTex.json");
	json j;
	input >> j;
	GenerateStageFromJSON(j);
	// 0を原点として、xとzを-540~540の間に配置する
	EnemiesController* pEnemiesController{ Instantiate<EnemiesController>(pPlayerPlane->GetEntityId()) };

	/*pEnemiesController->Spawan({ 0,30,10 });
	pEnemiesController->Spawan({ 0,30,300 });
	pEnemiesController->Spawan({ 0,30,-300 });
	pEnemiesController->Spawan({ 0,30,-600 });
	pEnemiesController->Spawan({ 0,30,600 });*/

	pEnemiesController->Spawan({ -540, 0, -440 });
	pEnemiesController->Spawan({ -340, 50, -470 });
	pEnemiesController->Spawan({ -280, 50, -470 });
	pEnemiesController->Spawan({ -100, 10, -480 });
	pEnemiesController->Spawan({ -20, 10, -480 });
	pEnemiesController->Spawan({ -440, 30, -390 });
	pEnemiesController->Spawan({ -420, 30, -340 });
	pEnemiesController->Spawan({ -400, 30, -390 });
	pEnemiesController->Spawan({ -100, 10, -360 });
	pEnemiesController->Spawan({ -50, 50, -410 });
	pEnemiesController->Spawan({ 240, 50, -360 });
	pEnemiesController->Spawan({ 210, 50, -400 });
	pEnemiesController->Spawan({ 310, 30, -290 });
	pEnemiesController->Spawan({ 360, 30, -240 });
	pEnemiesController->Spawan({ 470, 10, -300 });
	pEnemiesController->Spawan({ 510, 10, -260 });
	pEnemiesController->Spawan({ -450, 30, -90 });
	pEnemiesController->Spawan({ -420, 30, -90 });
	pEnemiesController->Spawan({ -140, 10, -130 });
	pEnemiesController->Spawan({ 160, 10, -90 });
	pEnemiesController->Spawan({ -435, 50, -40 });
	pEnemiesController->Spawan({ -320, 50, -5 });
	pEnemiesController->Spawan({ 440, 30, -60 });
	pEnemiesController->Spawan({ 520, 30, -60 });
	pEnemiesController->Spawan({ 480, 30, -20 });
	pEnemiesController->Spawan({ -480, 10, 140 });
	pEnemiesController->Spawan({ -100, 30, 140 });
	pEnemiesController->Spawan({ 70, 10, 190 });
	pEnemiesController->Spawan({ 170, 10, 180 });
	pEnemiesController->Spawan({ 210, 10, 180 });
	pEnemiesController->Spawan({ 320, 50, 175 });
	pEnemiesController->Spawan({ -440, 30, 320 });
	pEnemiesController->Spawan({ -420, 30, 320 });
	pEnemiesController->Spawan({ -190, 50, 315 });
	pEnemiesController->Spawan({ -140, 50, 315 });
	pEnemiesController->Spawan({ 0, 10, 310 });
	pEnemiesController->Spawan({ 160, 10, 330 });
	pEnemiesController->Spawan({ -340, 30, 460 });
	pEnemiesController->Spawan({ 180, 50, 420 });
	pEnemiesController->Spawan({ 200, 50, 450 });
	pEnemiesController->Spawan({ 220, 50, 480 });

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);

	Instantiate<SkySphere>();
	Instantiate<Reticle>(WindowContext::First);
	Instantiate<Reticle>(WindowContext::Second);

	timeLimit_ = Instantiate<TimeLimit>(180.0f);
	timeLimit_->StartTimer();
	timeLimit_->RegisterOnEndTimerCallback([]()
		{
			Game::System<SceneSystem>().Move<ResultScene>();
		});
	// 表示したいテキストを開始

	Instantiate<QuotaGauge>();

	// ラズパイと通信を開始
	ppiio_->Start(SERVER_IPEP);
	ppiio_->SendLED(PIIO::LED_STATUS::LEDS_NORMAL);
}

void PlayScene::Update()
{
	if (ledTimeLeft_ > 0.0f)
	{
		ledTimeLeft_ -= Time::DeltaTimeF();
		if (ledTimeLeft_ <= 0.0f)
		{
			ppiio_->SendLED(PIIO::LEDS_NORMAL);
		}
	}

	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<ResultScene>();
	}

	if (InputUtil::GetKeyDown(KeyCode::O))
	{
		//timeLimit_->StartTimer();
	}
	if (InputUtil::GetKeyDown(KeyCode::P))
	{
		//timeLimit_->PauseTimer();
	}
	if (InputUtil::GetKeyDown(KeyCode::I))
	{
		//timeLimit_->ResumeTimer();
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

void PlayScene::SetStatusWarnning()
{
	// ノーマル状態のときだけ警告モードに切り替える
	if (ppiio_->GetLedStatus() == PIIO::LEDS_NORMAL)
	{
		ledTimeLeft_ = LED_TIME_WARNNING;
		ppiio_->SendLED(PIIO::LEDS_WARNNING);
	}
}

void PlayScene::SetStatusNormal()
{
	ledTimeLeft_ = 0.0f;
}

void PlayScene::SetStatusClear()
{
	// クリアなら強制的にクリア状態にする
	//if (ppiio_->GetLedStatus() != PIIO::LEDS_WARNNING)
	{
		ledTimeLeft_ = LED_TIME_CLEAR;
		ppiio_->SendLED(PIIO::LEDS_CLEAR);
	}
}
