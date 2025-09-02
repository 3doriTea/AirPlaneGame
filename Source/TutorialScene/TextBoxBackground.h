#pragma once
#include <mtgb.h>

class TextBoxBackground : public mtgb::GameObject
{
public:
	TextBoxBackground();
	~TextBoxBackground();

	void Update() override;
	void Draw() const override;
private:
	ImageHandle hImage_;
};
