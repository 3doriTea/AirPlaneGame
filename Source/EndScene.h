#pragma once
#include <mtgb.h>
#include "TextBox.h"
#include "EndText.h"

class EndScene : public mtgb::GameScene
{
public:
	EndScene();
	~EndScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

private:

	CameraHandleInScene hCamera1_;
	CameraHandleInScene hCamera2_;
};