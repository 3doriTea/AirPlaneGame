#pragma once
#include "ImGuiShowable.h"
#include "mtgb.h"
class Player;
class PlayerProxy : public ImGuiShowable<Player>
{
public:
	PlayerProxy(Player* player);
	int* test1;
	float* test2;
	std::string name;
	Vector3* vec3_;
};