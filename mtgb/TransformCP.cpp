#include "TransformCP.h"


mtgb::TransformCP::TransformCP()
{
}

mtgb::TransformCP::~TransformCP()
{
}

void mtgb::TransformCP::Update()
{
	for (size_t i = 0; i < poolId_.size(); i++)
	{
		if (poolId_[i] != INVALD_ENTITY)
		{
			pool_[i].Compute();
		}
	}
}
