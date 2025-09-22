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

// 後方互換性のため、元のマクロも残す
#define LOGIMGUI(format,...) \
	//Game::System<Debug>().LogImGui("General",std::source_location::current(),format,__VA_ARGS__)

// カテゴリ指定版の新しいマクロ
#define LOGIMGUI_CAT(category, format, ...) \
	//Game::System<Debug>().LogImGui(category, std::source_location::current(), format, __VA_ARGS__)

#else

#define LOGF(format, ...)
#define LOGIMGUI(format, ...)
#define LOGIMGUI_CAT(category, format, ...)

#endif

namespace mtgb
{
	struct LogEntry
	{
		std::string msg;
		std::string msgDetail;
		std::string file;
		int line;
		std::string func;
		std::string objectName;
		std::string category;  // フィルター用カテゴリ
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
		/// ログをフォーマットして出力する
		/// </summary>
		/// <typeparam name="...Args">可変長引数型</typeparam>
		/// <param name="_format">フォーマット文字列</param>
		/// <param name="..._args">可変長引数</param>
		template<typename ...Args>
		static void LogF(const char* _format, const Args... _args);

		template<typename ...Args>
		void LogImGui(const std::string& category, const std::source_location& _location, const char* _format, const Args..._args);

		std::list<LogEntry> GetLog();

		static constexpr std::string_view GetName()
		{
			return "Log";
		}
	private:
		static constexpr size_t BUFFER_SIZE{ 1024 };  // ログ出力時の文字列バッファサイズ
		static constexpr UINT MAX_LOG_COUNT{ 30 };

		LogItr RemoveLog(LogItr itr);
		
		std::list<LogEntry> logs_;
		std::unordered_map<std::string, LogItr> logMap_;//ログがキー、logs_へのイテレータが値
		static std::string MakeKey(const std::string& object, const char* file, int line, const char* func, const std::string msg);
	};

	template<typename ...Args>
	inline void Debug::LogF(const char* _format, const Args... _args)
	{
		static char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // ヌル文字埋め

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);

		OutputDebugString(buffer);
	}

	template<typename ...Args>
	void Debug::LogImGui(const std::string& category, const std::source_location& _location, const char* _format, const Args..._args)
	{
		char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // ヌル文字埋め

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);
		
		std::string msg = mtgb::MultiToUTF8(buffer);
		std::string key = Debug::MakeKey(category, _location.file_name(), _location.line(), _location.function_name(), msg);
		key = mtgb::MultiToUTF8(key);
		auto itr = logMap_.find(key);
		if (itr != logMap_.end())
		{
			itr->second->count++;
		}
		else
		{
			logs_.push_back(
				LogEntry
				{
					.msg = msg,
					.msgDetail = key,
					.file = _location.file_name(),
					.line = static_cast<int>(_location.line()),
					.func = _location.function_name(),
					.objectName = "",  
					.category = category,  // カテゴリを設定
					.count = 1					
				});
			logMap_[key] = std::prev(logs_.end());
		}
	}

	
}
