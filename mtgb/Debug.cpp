#include "Debug.h"
#include "..\ImGui\imgui.h"



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
	if (logs_.size() > MAX_LOG_COUNT)
	{
		for (LogItr itr = logs_.begin(); itr != std::next(logs_.begin(), logs_.size() - MAX_LOG_COUNT);)
		{
			itr = RemoveLog(itr);
		}
	}
}

std::list<mtgb::LogEntry> mtgb::Debug::GetLog()
{
	return logs_;
}

mtgb::Debug::LogItr mtgb::Debug::RemoveLog(LogItr itr)
{
	logMap_.erase(itr->msg);
	return logs_.erase(itr);
}

std::string mtgb::Debug::MakeKey(const std::string& object, const char* file, int line, const char* func, const std::string msg)
{
	return msg + "|" + object + "|" + file + "|" + std::to_string(line) + func;
}
