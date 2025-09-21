#pragma once
#include <mtgb.h>

class IAutoPilot
{
public:
	IAutoPilot();
	virtual ~IAutoPilot();

	virtual bool TryUpdate() = 0;

private:
	Transform* pTransform_;
};
