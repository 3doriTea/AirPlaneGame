#include "Player.h"
#include "Bullet.h"
#include "../mtgb/DirectX11Draw.h"

using namespace mtgb;

namespace
{
	static const float PLAYER_SPEED{ 0.01f };
	int hText;
	int timer = 0;
	TimerHandle timerHandle;
}

Player::Player() : GameObject(GameObjectBuilder()
		.SetName("player")
		.SetPosition(Vector3(0, 0, 0))
		.SetRotate(Quaternion::Identity())
		.SetScale(Vector3::One())
		.Build()),
	pTransform_{ Component<Transform>() },
	pAudioPlayer_{ Component<AudioPlayer>() }
{
	test1 = 10;
	test2 = 20;
	str = "Hello,World!";
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	hImage_ = Image::Load("Image/player.png");
	hModel_ = OBJ::Load("Model/OBJ/cube.obj");
	hText = Text::Load(str,36);
	pTransform_->position_.z = 5.0f;
	//hMnow_ = Audio::Load("Sound/Meow.wav");
	//pAudioPlayer_->SetAudio(hMnow_);
	timerHandle = Timer::AddInterval(0.01, [this]() {timer += 10; });
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
	if (InputData::GetKeyDown(KeyCode::C))
	{
		Instantiate<Bullet>(pTransform_->position_);
		
	}

	if (InputData::GetKeyDown(KeyCode::F))
	{
		
	}

	if (InputData::GetKey(KeyCode::W))
	{
		pTransform_->position_ += pTransform_->Forward() * PLAYER_SPEED;
	}
	if (InputData::GetKey(KeyCode::S))
	{
		pTransform_->position_ += pTransform_->Down() * PLAYER_SPEED;
	}

	if (InputData::GetKey(KeyCode::A))
	{
		pTransform_->rotate_.f[2] += 1;
		//pTransform_->scale_.z -= 0.01f;
	}
	if (InputData::GetKey(KeyCode::D))
	{
		pTransform_->rotate_.f[2] -= 1;
		//pTransform_->scale_.z += 0.01f;
	}
}

void Player::Draw() const
{
	static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };
	Vector2Int mousePos = InputData::GetMousePosition();
	
	//Draw::Box({ SCREEN_SIZE.x / 2, SCREEN_SIZE.y / 2 }, { mousePos.x, mousePos.y }, Color::RED);

	RectInt draw{};
	draw.point = mousePos;
	draw.size = Image::GetSize(hImage_);
	
	Draw::OBJModel(hModel_, pTransform_);
	static int speed = 0;
	//MTImGui::ShowInspector(&speed, "speed");
	//Draw::Image(draw, { Vector2Int::Zero(), draw.size }, hImage_);
	//Draw::Image(hImage_, pTransform_);
	Game::System<Text>().ChangeFontSize(100);
	Draw::ImmediateText(std::to_string(timer),0,0);
	MTImGui::ShowInspector(this, "player");
	//Draw::Text(hText, mousePos);
}
