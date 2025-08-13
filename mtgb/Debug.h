#pragma once
#include "ISystem.h"
#include "IncludingWindows.h"
#include <string>
#include <unordered_map>
#include <list>
#include <source_location>
#include "MTStringUtility.h"
#ifdef _DEBUG

#define LOGF(format, ...) \
	mtgb::Debug::LogF(format, __VA_ARGS__)

//#define LOGIMGUI(objName,format,...) \
//	Game::System<Debug>().LogImGui(objName,std::source_location::current(),format,__VA_ARGS__)

#define LOGIMGUI(format,...) \
	Game::System<Debug>().LogImGui("",std::source_location::current(),format,__VA_ARGS__)

#else

#define LOGF(format, ...)

#endif

namespace mtgb
{
	struct LogEntry
	{
		std::string msg;
		/*std::string message;
		std::string file;
		int line;
		std::string func;
		std::string objectName;*/
		int count = 1;
	};

	class Debug : public ISystem
	{
	public:
		using LogItr = std::list<LogEntry>::iterator;
		Debug();
		~Debug();

		void Initialize() override;
		void Update() override;
	public:
		/// <summary>
		/// ���O���t�H�[�}�b�g���ďo�͂���
		/// </summary>
		/// <typeparam name="...Args">�ϒ������^</typeparam>
		/// <param name="_format">�t�H�[�}�b�g������</param>
		/// <param name="..._args">�ϒ�����</param>
		template<typename ...Args>
		static void LogF(const char* _format, const Args... _args);

		template<typename ...Args>
		void LogImGui(const std::string& object, const std::source_location& _location, const char* _format, const Args..._args);

		std::list<LogEntry> GetLog();
	private:
		static constexpr size_t BUFFER_SIZE{ 1024 };  // ���O�o�͎��̕�����o�b�t�@�T�C�Y
		static constexpr UINT MAX_LOG_COUNT{ 30 };

		LogItr RemoveLog(LogItr itr);
		

		std::list<LogEntry> logs_;
		std::unordered_map<std::string, LogItr> logMap_;//���O���L�[�Alogs_�ւ̃C���f�b�N�X���l
		static std::string MakeKey(const std::string& object, const char* file, int line, const char* func, const std::string msg);
	};

	template<typename ...Args>
	inline void Debug::LogF(const char* _format, const Args... _args)
	{
		static char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // �k����������

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);

		OutputDebugString(buffer);
	}
	template<typename ...Args>
	void Debug::LogImGui(const std::string& object, const std::source_location& _location, const char* _format, const Args..._args)
	{
		char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // �k����������

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);
		
		std::string msg = buffer;
		std::string key = Debug::MakeKey(object, _location.file_name(), _location.line(), _location.function_name(), msg);
		key = mtgb::MultiToUTF8(key);
		auto itr = logMap_.find(key);
		if (itr != logMap_.end())
		{
			itr->second->count++;
		}
		else
		{
			logs_.push_back({key});
			logMap_[key] = std::prev(logs_.end());
		}
	}
}
