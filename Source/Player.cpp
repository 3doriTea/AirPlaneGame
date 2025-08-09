#include "Player.h"
#include "Bullet.h"
#include "../mtgb/DirectX11Draw.h"
#include "DoubleWindow.h"
#include "Inspector.h"
using namespace mtgb;

namespace
{
	static const float PLAYER_MOVE_SPEED{ 0.1f };
	static const float PLAYER_MAX_SPEED{ 100.0f };
	static const float PLAYER_MIN_SPEED{ 0.0f };
	int hText;
	int timer = 0;
	TimerHandle timerHandle;
	
}

Player::Player() : GameObject(GameObjectBuilder()
		.SetName("player")
		.SetPosition(Vector3(0, 0, 0))
		.SetRotate(Quaternion::Identity())
		.SetScale({ 1, 1, 1 })
		.Build()),
	pTransform_{ Component<Transform>() },
	pAudioPlayer_{ Component<AudioPlayer>() },
	pRigidbody_{Component<RigidBody>()},
	pCamera_{nullptr}
{
	
	acceleration_ = 0.0f;
	name_ = "Player:" + std::to_string(entityId_);
	test1 = 10;
	test2 = 20;
	str = "Hello,World!";
	//DirectX11Draw::SetIsWriteToDepthBuffer(false);
	//hImage_ = Image::Load("Image/player.png");
	//hModel_ = OBJ::Load("Model/OBJ/cube.obj");
	fModel_ = Fbx::Load("Model/tCube.fbx");
	hText = Text::Load(str,36);
	pTransform_->position.z = 5.0f;
	pTransform_->scale = Vector3(1, 1, 1);
	//hMnow_ = Audio::Load("Sound/Meow.wav");
	//pAudioPlayer_->SetAudio(hMnow_);
	timerHandle = Timer::AddInterval(0.01f, [this]() { timer += 10; });
	//pCamera_ = FindGameObject();
}

Player::Player(mtgb::WindowContext context)
	:Player()
{
	context_ = context;
}

Player::~Player()
{
}

void Player::Update()
{
	if (timer > 100000)
	{
		timer = 0;
	}
	if (InputUtil::GetKeyDown(KeyCode::Space,context_))
	{
		Instantiate<Bullet>(pTransform_->position);
	}

	if (InputUtil::GetKeyDown(KeyCode::F,context_))
	{
		
	}

	if (InputUtil::GetKey(KeyCode::W,context_))
	{
		//pRigidbody_->velocity_ += pTransform_->Forward() * PLAYER_MOVE_SPEED;
		acceleration_ = (std::min)(acceleration_ + PLAYER_MOVE_SPEED , PLAYER_MAX_SPEED);
	}
	if (InputUtil::GetKey(KeyCode::S,context_))
	{
		//pRigidbody_->velocity_ += pTransform_->Back() * PLAYER_MOVE_SPEED;
		acceleration_ = (std::max)(acceleration_ - PLAYER_MOVE_SPEED, PLAYER_MIN_SPEED);
		//pTransform_->position += pTransform_->Down() * PLAYER_SPEED;
	}
	//pRigidbody_->velocity_ *= pTransform_->Forward();
	if (InputUtil::GetKey(KeyCode::A,context_))
	{
		pTransform_->rotate.f[2] += 1;
		//pTransform_->scale_.z -= 0.01f;
	}
	if (InputUtil::GetKey(KeyCode::D,context_))
	{
		pTransform_->rotate.f[2] -= 1;
		//pTransform_->scale_.z += 0.01f;
	}
	if (InputUtil::GetKey(KeyCode::E))
	{
		pTransform_->rotate.f[1] -= 1;
		//pTransform_->scale_.z += 0.01f;
	}
	if (InputUtil::GetKey(KeyCode::Q))
	{
		pTransform_->rotate.f[1] += 1;
		//pTransform_->scale_.z += 0.01f;
	}
	pRigidbody_->velocity_ = pTransform_->Forward() * acceleration_;
}

void Player::Draw() const
{
	static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	Vector2Int mousePos = InputUtil::GetMousePosition(context_);
	
	//Draw::Box({ SCREEN_SIZE.x / 2, SCREEN_SIZE.y / 2 }, { mousePos.x, mousePos.y }, Color::RED);

	//Draw::OBJModel(hModel_, pTransform_);
	Draw::FBXModel(fModel_, *pTransform_, 300);
	static int speed = 0;
	//MTImGui::ShowInspector(&speed, "speed");
	//Draw::Image(draw, { Vector2Int::Zero(), draw.size }, hImage_);
	
	Game::System<Text>().ChangeFontSize(100);
	Draw::ImmediateText(std::to_string(timer),0,0);
	//MTImGui::ShowInspector(this, "player");
	//Draw::Text(hText, mousePos);
	TypeRegistry::ShowInspector(this, name_.c_str());
}
