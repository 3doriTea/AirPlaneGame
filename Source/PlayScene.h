#pragma once
#include "GameScene.h"
#include "TextBox.h"

namespace Network
{
	class PIIO;
}

class PlayScene : public mtgb::GameScene
{
public:
	PlayScene();
	~PlayScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

private:
	TextBox* tBox_;
	Network::PIIO* ppiio_;
};
