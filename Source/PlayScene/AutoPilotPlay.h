#pragma once
#include "../IAutoPilot.h"

class AutoPilotPlay : public IAutoPilot
{
public:
	AutoPilotPlay() {};
	~AutoPilotPlay() {};

	bool TryUpdate() override;
};
