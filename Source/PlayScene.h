#pragma once
#include "GameScene.h"
#include "TextBox.h"

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
	CameraHandleInScene hCamera1_;
	CameraHandleInScene hCamera2_;
};
