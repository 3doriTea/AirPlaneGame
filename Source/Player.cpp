#include "Player.h"
#include "Bullet.h"
#include "../mtgb/DirectX11Draw.h"

using namespace mtgb;

namespace
{
	static const float PLAYER_SPEED{ 0.01f };
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
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	hImage_ = Image::Load("Image/player.png");
	hModel_ = OBJ::Load("Model/OBJ/cube.obj");
	pTransform_->position_.z = 5.0f;
	//hMnow_ = Audio::Load("Sound/Meow.wav");
	//pAudioPlayer_->SetAudio(hMnow_);
}

Player::~Player()
{
}

void Player::Update()
{
	if (InputData::GetKeyDown(KeyCode::C))
	{
		Instantiate<Bullet>(pTransform_->position_);
		//pAudioPlayer_->Play();
		//Audio::PlayOneShotFile("Sound/Meow.wav");
		//massert(false);
	}

	if (InputData::GetKeyDown(KeyCode::F))
	{
		//Audio::PlayOneShotFile("Sound/080415pianobgm3popver.wav");
	}

	if (InputData::GetKey(KeyCode::W))
	{
		pTransform_->position_ += pTransform_->Forward() * PLAYER_SPEED;
		//pTransform_->position_ += pTransform_->Up() * PLAYER_SPEED;
		//pTransform_->position_ += pTransform_->Up() + 
		//LOGF("y=%f\n", pTransform_->position_.y);
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
	//Draw::Image(draw, { Vector2Int::Zero(), draw.size }, hImage_);
	//Draw::Image(hImage_, pTransform_);

	//testme(Naohiro)
}
