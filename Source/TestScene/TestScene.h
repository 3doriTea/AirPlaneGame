#pragma once
#include <mtgb.h>

class IAutoPilot;

class TestScene : public mtgb::GameScene
{
public:
	TestScene();
	~TestScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
private:
	IAutoPilot* pAutoPilot_;  // プレイヤー自動操縦機能
};
