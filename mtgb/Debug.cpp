#include "Debug.h"
#include "..\ImGui\imgui.h"

std::deque<mtgb::LogEntry> mtgb::Debug::logs_;

mtgb::Debug::Debug()
{
}

mtgb::Debug::~Debug()
{
}

void mtgb::Debug::Initialize()
{
	
}

void mtgb::Debug::Update()
{
}

void mtgb::Debug::ShowLogWindow()
{
	for (const auto& log : logs_)
	{
		ImGui::Text("%s", log.message.c_str());
	}
	if (logs_.size() > MAX_LOG_COUNT)
	{
		logs_.resize(MAX_LOG_COUNT);
	}
}
