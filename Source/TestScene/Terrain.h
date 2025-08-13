#pragma once
#include <mtgb.h>

class Terrain : public mtgb::GameObject
{
public:
	Terrain();
	~Terrain();

	void Update() override;
	void Draw() const override;

private:

};
