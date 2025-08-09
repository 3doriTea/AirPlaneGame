#pragma once
#include "ReflProxy.h"
class Player;
class PlayerProxy : public ReflProxy<Player>
{
public:
	PlayerProxy(Player* player);
	int* test1;
	float* test2;
	std::string name;
	int a;
};