#include "PlayScene.h"
#include "Player.h"
#include "Enemy.h"
#include "Camera.h"
#include "Network/PIIO.h"
//#include "MTNet/Common.h"


using namespace mtgb;
using Network::PIIO;

namespace
{
	static const mtnet::IPEndPoint LOCAL_IPEP{ "192.168.42.183", 60349 };
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
	SetCameraGameObject(Instantiate<Camera>());

	Instantiate<Player>();
	Instantiate<Enemy>();

	// 表示したいテキストを開始
	tBox_->SetTextSpeedSec(0.1f);
	tBox_->Show("hello this test");
}

void PlayScene::Update()
{
	if (InputData::GetKeyDown(KeyCode::Escape))
	{
		Game::Exit();
	}
	//LOGF("テスト");

	if (InputData::GetKeyDown(KeyCode::F))
	{
		ppiio_->Start(SERVER_IPEP);
	}
}

void PlayScene::Draw() const
{
	tBox_->Draw();
}

void PlayScene::End()
{
}
