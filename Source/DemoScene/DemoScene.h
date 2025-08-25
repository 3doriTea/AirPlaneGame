#pragma once
#include <mtgb.h>

class DemoScene : public mtgb::GameScene
{
public:
	DemoScene();
	~DemoScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

private:
	
};
