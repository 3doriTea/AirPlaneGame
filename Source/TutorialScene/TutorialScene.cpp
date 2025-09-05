#include "TutorialScene.h"

#include "../PlayScene.h"
#include "../SkySphere.h"
#include "../Reticle.h"

#include "../TestScene/Terrain.h"
#include "../TestScene/PlayerPlane.h"
#include "../TestScene/PlayerGunner.h"
#include "../TestScene/PlayerPilot.h"
#include "../TestScene/UI/Radar.h"

#include "TextBoxBackground.h"
#include "TextBoxTimeBar.h"
#include "SpeechQueue.h"

namespace
{
	// プレイシーンに遷移するまでの時間(秒)
	const float TO_PLAY_SCENE_WAIT_SEC{ 60 };

	const float TEXT_BOX_START_WAIT_TIME{ 3 };

	// 台本
	// MEMO: Visual Studio の場合、tab文字で幅統一できる
	SpeechQueue speechQueue
	{ {
		{ u8"こんにちは。オペレーターだよ。",								"Sound/Voice/001_ずんだもん（ノーマル）_こんにちは。オペレ….wav",	4.0 },  // 0
		{ u8"まもなく、敵がいる地点に到着するよ。",							"Sound/Voice/002_ずんだもん（ノーマル）_まもなく、敵がいる….wav",	4.0 },  // 1
		{ u8"その前に、操作に慣れておこう。",								"Sound/Voice/003_ずんだもん（ノーマル）_その前に、操作に慣….wav",	4.0 },  // 2
		{ u8"君たちの任務は、協力して敵を撃破することだよ。",					"Sound/Voice/004_ずんだもん（ノーマル）_諸君の任務は、協力….wav",	5.0 },  // 3
		{ u8"時間内にノルマを達成するんだよ。",								"Sound/Voice/005_ずんだもん（ノーマル）_時間内にノルマを達….wav",	4.0 },  // 4
		{ u8"←側は、射撃手、敵をバンバン撃ってね。",							"Sound/Voice/006_ずんだもん（ノーマル）_左の君は、射撃手、….wav",	6.0 },  // 5
		{ u8"→側は、運転手、飛行機を操縦して、好きなところに向かってね。",		"Sound/Voice/007_ずんだもん（ノーマル）_右の君は、運転手、….wav",	7.0 },  // 6
		{ u8"2人とも、スティックを倒して、方向を変えられるよ。",				"Sound/Voice/008_ずんだもん（ノーマル）_2人とも、スティッ….wav",	5.0 },  // 7
		{ u8"トリガーを押すと、弾を発射できるよ。",							"Sound/Voice/009_ずんだもん（ノーマル）_トリガーを押すと、….wav",	5.0 },  // 8
		{ u8"←側の運転手は、スライダーを動かして飛行機の速度を変えられるよ。",	"Sound/Voice/010_ずんだもん（ノーマル）_右の、運転手は、ス….wav",	7.0 },  // 9
		//{ u8"そろそろ敵が見えてくるはず、幸運を祈るよ。", "", 0 },
	} };
}

TutorialScene::TutorialScene() :
	hToPlaySceneTimer_{ nullptr },
	textBoxToChangeTimeLeft_{ TEXT_BOX_START_WAIT_TIME }
{
}

TutorialScene::~TutorialScene()
{
}

void TutorialScene::Initialize()
{
	TypeRegistry::Instance();
	TypeRegistry::Instance().Initialize();
	MTImGui::Instance().Initialize();

	// 一定時間経ったら必ずプレイシーンに遷移
	hToPlaySceneTimer_ = Timer::AddAram(TO_PLAY_SCENE_WAIT_SEC, []()
		{
			Game::System<SceneSystem>().Move<PlayScene>();
		});

	state_
		.OnUpdate(S_HANDSHAKE, []
		{
		});

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

	pTextBox_ = Instantiate<TextBox>(0.01f, GenDrawScreenFrom(Vector2Int{ 160, 480 }), 24);
	pTextBox_->SetPopTimeSec(0.1f);
	pTextBox_->SetUIParams({ 6 });

	pTextBoxBackground_ = Instantiate<TextBoxBackground>();
	pTextBoxTimeBar_ = Instantiate<TextBoxTimeBar>();
}

void TutorialScene::Update()
{
	if (InputUtil::GetKeyDown(KeyCode::T))
	{
		Game::System<SceneSystem>().Move<PlayScene>();
	}

	if (speechQueue.IsFinished())
	{
		return;  // 台本読み終わっているなら回帰
	}

	textBoxToChangeTimeLeft_ -= Time::DeltaTimeF();
	if (textBoxToChangeTimeLeft_ <= 0.0f)
	{
		SPEECH_ELEMENT element{};
		if (speechQueue.TryGetNext(element))
		{
			textBoxToChangeTimeLeft_ += element.time_;
			pTextBoxTimeBar_->SetTimeLeftMax(element.time_);
			pTextBox_->Show(element.text_.data());
			Game::System<Audio>().PlayOneShotFile(element.audioFile_.data());
		}
	}

	state_.Update();
}

void TutorialScene::Draw() const
{
}

void TutorialScene::End()
{
}
