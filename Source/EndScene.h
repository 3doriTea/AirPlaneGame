#pragma once
#include <mtgb.h>

class EndScene : public mtgb::GameScene
{
public:
	EndScene();
	~EndScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;
};