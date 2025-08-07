#pragma once
#include <mtgb.h>

class TestScene : public mtgb::GameScene
{
public:
	TestScene();
	~TestScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
};
