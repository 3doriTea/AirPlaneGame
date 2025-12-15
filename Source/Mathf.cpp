#include "pch\pch.h"
#include "Mathf.h"

wtgb::Mathf::Randomer::Randomer(const uint32_t _seed) :
	randomEngine_{ _seed }
{
}

wtgb::Mathf::Randomer::Randomer() :
	Randomer{ std::random_device{}() }
{
}

wtgb::Mathf::Randomer::~Randomer()
{
}

int wtgb::Mathf::Randomer::NextInt(int _max)
{
	std::uniform_int_distribution<int> dist{ 0, _max };
	return dist(randomEngine_);
}

float wtgb::Mathf::Randomer::Rand()
{
	std::uniform_real_distribution<float> dist{ 0.0f, 1.0f };
	return dist(randomEngine_);
}
