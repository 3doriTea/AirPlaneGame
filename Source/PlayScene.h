#pragma once
#include "GameScene.h"

class PlayScene : public mtgb::GameScene
{
public:
	PlayScene();
	~PlayScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
};
