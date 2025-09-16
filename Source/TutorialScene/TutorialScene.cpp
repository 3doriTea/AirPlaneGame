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
#include "ImageAnimator.h"
#include "ToPlayTimer.h"

namespace
{
	// プレイシーンに遷移するまでの時間(秒)
	const float TO_PLAY_SCENE_WAIT_SEC{ 60 };

	const float TEXT_BOX_START_WAIT_TIME{ 3 };

	// 台本
	SpeechQueue speechQueue
	{ {
		// MEMO: Visual Studio の場合、tab文字で幅統一できる
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
		{ u8"そろそろ到着するよ！幸運を祈るよ！",							"Sound/Voice/011_ずんだもん（ノーマル）_そろそろ到着するよ….wav", 4.0 },
	} };

	// フライトスティックコントローラーの画像ファイル名
	const std::string CONTROLLER_ANIM_IMAGE_FILE_A[TutorialScene::CI_MAX]
	{
		"Image/Hint-PitchDown.png",
		"Image/Hint-PitchUp.png",
		"Image/Hint-YawRight.png",
		"Image/Hint-YawLeft.png",
	};

	const std::string SIDE_IMAGE_FILE_A{ "LeftSide.png" };
	const std::string SIDE_IMAGE_FILE_B{ "RightSide.png" };

	// コントローラーアニメーションヒントの描画範囲
	const RectF DRAW_RECT_CON_ANIM_HINT{ 760.0f, 120.0f, 400.0f, 520.0f };
	// 字幕の描画座標
	const Vector2Int DRAW_POS_TEXT_BOX{ 320, 820 };
	// 字幕のフォントサイズ
	const int DRAW_FONT_TEXT_BOX{ 24 };
	// コントローラーアニメーションヒントの1フレーム時間
	const float FRAME_TIME_SEC_CON_ANIM{ 0.5f };
}

TutorialScene::TutorialScene() :
	hToPlaySceneTimer_{ nullptr },
	textBoxToChangeTimeLeft_{ TEXT_BOX_START_WAIT_TIME },
	pImageAnimatorA_{ nullptr },
	pImageAnimatorB_{ nullptr },
	hControllerImagesA_{}
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

	// コントローラーアニメーションヒントの画像読み込みA
	hControllerImagesA_.resize(CI_MAX);
	for (int i = 0; i < CI_MAX; i++)
	{
		hControllerImagesA_[i] = Image::Load(CONTROLLER_ANIM_IMAGE_FILE_A[i]);
	}

	// コントローラーアニメーションヒントの画像読み込みB
	hControllerImagesB_.resize(CI_MAX);
	for (int i = 0; i < CI_MAX; i++)
	{
		// TODO: 画像準備でき次第Bに変更
		hControllerImagesB_[i] = Image::Load(CONTROLLER_ANIM_IMAGE_FILE_A[i]);
		// hControllerImagesB_[i] = Image::Load(CONTROLLER_ANIM_IMAGE_FILE_B[i]);
	}

	hSideImageA_ = Image::Load(SIDE_IMAGE_FILE_A);
	hSideImageB_ = Image::Load(SIDE_IMAGE_FILE_B);

	// 一定時間経ったら必ずプレイシーンに遷移
	hToPlaySceneTimer_ = Timer::AddAram(TO_PLAY_SCENE_WAIT_SEC, []()
		{
			Game::System<SceneSystem>().Move<PlayScene>();
		});

	state_
		.OnStart(S_STICK, [this]
			{
				massert(pImageAnimatorA_ == nullptr && "AnimatorAが消されていない");
				massert(pImageAnimatorB_ == nullptr && "AnimatorBが消されていない");

				pImageAnimatorA_ = Instantiate<ImageAnimator>(
					ImageAnimator::Setting
					{
						.drawRect_ = DRAW_RECT_CON_ANIM_HINT,
						.defaultTimeSec_ = FRAME_TIME_SEC_CON_ANIM,
						.elements_ = hControllerImagesA_,
						.uIParams_ = { 10 }
					},
					GameObjectLayer::A);
				pImageAnimatorB_ = Instantiate<ImageAnimator>(
					ImageAnimator::Setting
					{
						.drawRect_ = DRAW_RECT_CON_ANIM_HINT,
						.defaultTimeSec_ = FRAME_TIME_SEC_CON_ANIM,
						.elements_ = hControllerImagesB_,
						.uIParams_ = { 10 }
					},
					GameObjectLayer::B);
			})
		.OnEnd(S_STICK, [this]
			{
				pImageAnimatorA_->DestroyMe();
				pImageAnimatorB_->DestroyMe();
				pImageAnimatorA_ = nullptr;
				pImageAnimatorB_ = nullptr;
			})
		.OnStart(S_RIGHT_SIDE, [this]
			{
				massert(pImageAnimatorB_ == nullptr && "AnimatorBが消されていない");
				pImageAnimatorB_ = Instantiate<ImageAnimator>(
					ImageAnimator::Setting
					{
						.drawRect_ = DRAW_RECT_CON_ANIM_HINT,
						.defaultTimeSec_ = FRAME_TIME_SEC_CON_ANIM,
						.elements_ = { { hSideImageB_ } },
						.uIParams_ = { 10 }
					},
					GameObjectLayer::B);
			})
		.OnEnd(S_RIGHT_SIDE, [this]
			{
				pImageAnimatorB_->DestroyMe();
				pImageAnimatorB_ = nullptr;
			})
		.OnStart(S_LEFT_SIDE, [this]
			{
				massert(pImageAnimatorA_ == nullptr && "AnimatorAが消されていない");
				pImageAnimatorA_ = Instantiate<ImageAnimator>(
					ImageAnimator::Setting
					{
						.drawRect_ = DRAW_RECT_CON_ANIM_HINT,
						.defaultTimeSec_ = FRAME_TIME_SEC_CON_ANIM,
						.elements_ = { { hSideImageA_ } },
						.uIParams_ = { 10 }
					},
					GameObjectLayer::A);

			})
		.OnEnd(S_LEFT_SIDE, [this]
			{
				pImageAnimatorA_->DestroyMe();
				pImageAnimatorA_ = nullptr;
			})
		;

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

	pTextBox_ = Instantiate<TextBox>(0.01f, GenDrawScreenFrom(DRAW_POS_TEXT_BOX), GenDrawScreenFontSize(DRAW_FONT_TEXT_BOX));
	pTextBox_->SetPopTimeSec(0.1f);
	pTextBox_->SetUIParams({ 6 });

	pTextBoxBackground_ = Instantiate<TextBoxBackground>();
	pTextBoxTimeBar_ = Instantiate<TextBoxTimeBar>();
	Instantiate<ToPlayTimer>(TO_PLAY_SCENE_WAIT_SEC);
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

	state_.Update();

	textBoxToChangeTimeLeft_ -= Time::DeltaTimeF();
	if (textBoxToChangeTimeLeft_ <= 0.0f)
	{
		SPEECH_ELEMENT element{};
		if (speechQueue.TryGetNext(element))
		{
			textBoxToChangeTimeLeft_ += element.time_;
			// 字幕タイマーセット！
			pTextBoxTimeBar_->SetTimeLeftMax(element.time_);
			// 字幕表示！
			pTextBox_->Show(element.text_.data());
			// テキスト読み上げる！
			Game::System<Audio>().PlayOneShotFile(element.audioFile_.data());
			// 次のステータスに変更
			STATE nextState{ static_cast<STATE>(speechQueue.GetCurrentLine()) };
			state_.Change(nextState);
		}
	}
}

void TutorialScene::Draw() const
{
}

void TutorialScene::End()
{
}
