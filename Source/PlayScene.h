#pragma once
#include "GameScene.h"
#include "TextBox.h"
#include "TerrainReader.h"

namespace Network
{
	class PIIO;
}
class TimeLimit;
class IAutoPilot;

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
	TimeLimit* timeLimit_;
	Network::PIIO* ppiio_;
	int val_;

	CameraHandleInScene hCamera1_;
	CameraHandleInScene hCamera2_;

	TerrainReader8* pReader8_;
	IAutoPilot* pAutoPilot_;  // プレイヤー自動操縦機能
};
