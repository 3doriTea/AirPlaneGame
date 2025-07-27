#include "Log.h"
// ƒXƒgƒŠ[ƒ€•Ö—˜Œn
#include <bitset>
#include <iomanip>
#include <sstream>
#include <ios>

void mtnet::Log::Write(const std::string& content)
{
	logs_.push_back(content);
	if (logs_.size() >= LOGS_COUNT_MAX)
	{
		logs_.erase(logs_.begin());
	}

	{  // ”r‘¼§Œä
		std::lock_guard<std::mutex> lock(logWriteMutex_);
		onLogFunc_("log:" + content);
	}
}

void mtnet::Log::Write(const char* _binArray, const size_t& _binArraySize)
{
	std::stringstream ss{};

	for (int i = 0; i < _binArraySize; i++)
	{
		int byteValue{ _binArray[i] };
		ss << std::hex << std::setw(2) << std::setfill('0') << byteValue << " ";
	}

	Write(ss.str());
}

std::string mtnet::Log::GenerateLogs()
{
	std::stringstream stream{};

	for (auto&& log : logs_)
	{
		stream << log << std::endl;
	}

	return stream.str();
}

void mtnet::Log::OnLog(const OnLogFunc& _callback)
{
	onLogFunc_ = _callback;
}

std::list<std::string> mtnet::Log::logs_{};
mtnet::OnLogFunc mtnet::Log::onLogFunc_{};
std::mutex mtnet::Log::logWriteMutex_{};
