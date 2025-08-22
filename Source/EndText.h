#pragma once
#include <mtgb.h>

class EndText : public GameObject
{
public:
	EndText();
	~EndText();
	void Update() override;
	void Draw() const override;

};