#include "DemoScene.h"
#include "../CheckTutorialScene/CheckTutorialScene.h"
#include "../TestScene/TestScene.h"

#include "HangerUI.h"
#include "CameraTripod.h"

using namespace mtgb;

namespace
{
	const float TO_MOVE_TIME_SEC{ 3.0f };
}

DemoScene::DemoScene() :
	pHangerUIA_{ nullptr },
	pHangerUIB_{ nullptr },
	toMoveTimeLeft_{ TO_MOVE_TIME_SEC }
{
}

DemoScene::~DemoScene()
{
}

void DemoScene::Initialize()
{
	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	Audio::Clear();

	// âEë§ÇÃâ^ì]éË
	CameraTripod* pTripodA = Instantiate<CameraTripod>(Vector3{ 3, 0, 0 }, Quaternion::Identity());
	CameraHandleInScene hCamera1 = RegisterCameraGameObject(pTripodA);
	//EntityId eIdTripodA{ pTripodA->GetEntityId() };

	// ç∂ë§ÇÃéÀåÇéË
	CameraTripod* pTripodB = Instantiate<CameraTripod>(Vector3{ -3, 0, 0 }, Quaternion::Identity());
	CameraHandleInScene hCamera2 = RegisterCameraGameObject(pTripodB);
	//EntityId eIdTripodB{ pTripodB->GetEntityId() };

	WinCtxRes::Get<CameraResource>(WindowContext::First).SetHCamera(hCamera1);
	WinCtxRes::Get<CameraResource>(WindowContext::Second).SetHCamera(hCamera2);
	
	pHangerUIA_ = Instantiate<HangerUI>(GameObjectLayer::A);
	pHangerUIB_ = Instantiate<HangerUI>(GameObjectLayer::B);
}

void DemoScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<TestScene>();
	}

	if (pHangerUIA_->IsOpennd() && pHangerUIB_->IsOpennd())
	{
		toMoveTimeLeft_ -= Time::DeltaTimeF();
		if (toMoveTimeLeft_ <= 0.0f)
		{
			Game::System<SceneSystem>().Move<CheckTutorialScene>();
		}
	}
	else
	{
		toMoveTimeLeft_ = TO_MOVE_TIME_SEC;
	}
}

void DemoScene::Draw() const
{
}

void DemoScene::End()
{
}
