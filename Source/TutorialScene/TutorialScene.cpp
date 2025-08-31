#include "TutorialScene.h"

#include "../PlayScene.h"
#include "../SkySphere.h"
#include "../Reticle.h"

#include "../TestScene/Terrain.h"
#include "../TestScene/PlayerPlane.h"
#include "../TestScene/PlayerGunner.h"
#include "../TestScene/PlayerPilot.h"
#include "../TestScene/UI/Radar.h"

namespace
{
	// プレイシーンに遷移するまでの時間(秒)
	const float TO_PLAY_SCENE_WAIT_SEC{ 60 };
}

TutorialScene::TutorialScene() :
	hToPlaySceneTimer_{ nullptr }
{
}

TutorialScene::~TutorialScene()
{
}

void TutorialScene::Initialize()
{
	// 一定時間経ったら必ずプレイシーンに遷移
	hToPlaySceneTimer_ = Timer::AddAram(TO_PLAY_SCENE_WAIT_SEC, []()
		{
			Game::System<SceneSystem>().Move<PlayScene>();
		});

	state_
		.OnUpdate(S_HANDSHAKE, []
		{
		});

	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Audio::Clear();

	Instantiate<SkySphere>();
	Instantiate<Terrain>();
	Instantiate<Reticle>();

	PlayerPlane* pPlayerPlane{ Instantiate<PlayerPlane>() };
	EntityId eIdPlayer{ pPlayerPlane->GetEntityId() };

	PlayerPilot* pPilot{ Instantiate<PlayerPilot>(eIdPlayer) };
	CameraHandleInScene hCamera1 = RegisterCameraGameObject(pPilot);

	PlayerGunner* pGunner{ Instantiate<PlayerGunner>(eIdPlayer) };
	CameraHandleInScene hCamera2 = RegisterCameraGameObject(pGunner);

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);

	Instantiate<Radar>(eIdPlayer, GameObjectLayer::A);
	Radar* pGunnerRader{ Instantiate<Radar>(eIdPlayer, GameObjectLayer::B) };
	pGunner->SetRadarUI(pGunnerRader);
}

void TutorialScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<PlayScene>();
	}

	state_.Update();
}

void TutorialScene::Draw() const
{
}

void TutorialScene::End()
{
}
