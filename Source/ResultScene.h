#pragma once
#include <mtgb.h>
#include "TextBox.h"

class ResultScene : public mtgb::GameScene
{
public:
	ResultScene();
	~ResultScene();

	void Initialize() override;
	void Update() override;
	void Draw() const override;
	void End() override;

private:

	CameraHandleInScene hCamera1_;
	CameraHandleInScene hCamera2_;
};