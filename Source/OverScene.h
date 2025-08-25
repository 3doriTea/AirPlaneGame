#pragma once
#include <mtgb.h>

class OverScene : public mtgb::GameScene
{
public:
	OverScene();
	~OverScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
};