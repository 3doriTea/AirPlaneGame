#pragma once
#include "ImGuiShowable.h"
#include "mtgb.h"
class Player;
class TransformProxy;
class PlayerProxy : public ImGuiShowable<Player>
{
public:
	PlayerProxy(Player* player);
	int* test1;
	float* test2;
	std::string_view name;
	TransformProxy* pTransformProxy_;
};