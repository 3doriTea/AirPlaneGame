#pragma once
#include "ISystem.h"
#include "IncludingWindows.h"
#include <string>
#include <chrono>
#include <deque>

#ifdef _DEBUG

#define LOGF(format, ...) \
	mtgb::Debug::LogF(format, __VA_ARGS__)

#define LOGIMGUI(format,...) \
	mtgb::Debug::LogImGui(format,__VA_ARGS__)

#else

#define LOGF(format, ...)

#endif

namespace mtgb
{
	struct LogEntry
	{
		std::string message;
		std::chrono::steady_clock::time_point timestamp;
	};

	class Debug : public ISystem
	{
	public:
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
		static void LogImGui(const char* _format, const Args..._args);

		static void ShowLogWindow();
	private:
		static constexpr size_t BUFFER_SIZE{ 1024 };  // ログ出力時の文字列バッファサイズ
		static constexpr UINT MAX_LOG_COUNT{ 30 };

		
		static std::deque<LogEntry> logs_;
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
	void Debug::LogImGui(const char* _format, const Args ..._args)
	{
		char buffer[BUFFER_SIZE]{};
		ZeroMemory(buffer, BUFFER_SIZE);  // ヌル文字埋め

		::sprintf_s<BUFFER_SIZE>(buffer, _format, _args...);
		
		logs_.push_front({ buffer,std::chrono::steady_clock::now() });
	}
}
