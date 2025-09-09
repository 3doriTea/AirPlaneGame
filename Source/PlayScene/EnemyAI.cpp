#include "EnemyAI.h"

EnemyAI::EnemyAI()
{
}

void EnemyAI::Update()
{
}

void EnemyAI::SetInput(InputData&& _data)
{
	inputData_ = std::move(_data);
}
