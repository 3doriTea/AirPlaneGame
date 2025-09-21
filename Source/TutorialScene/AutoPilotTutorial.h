#pragma once
#include "../IAutoPilot.h"

/// <summary>
/// チュートリアル時の自動操縦機能
/// </summary>
class AutoPilotTutorial : public IAutoPilot
{
public:
	using IAutoPilot::IAutoPilot;
	~AutoPilotTutorial() {};

	bool TryUpdate() override;
};
