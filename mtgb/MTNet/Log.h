#pragma once
#include <string>
#include <list>
#include <functional>
#include <mutex>

namespace mtnet
{
	constexpr size_t LOGS_COUNT_MAX{ 100 };  // 蓄積するログの最大数
	using OnLogFunc = std::function<void(const std::string&)>;

	/// <summary>
	/// データキャリアクラス
	/// </summary>
	class Log
	{
	public:
		/// <summary>
		/// ログを書き込む
		/// </summary>
		/// <param name="_content">内容</param>
		static void Write(const std::string& _content);

		/// <summary>
		/// バイナリ配列をログに書き込む
		/// </summary>
		/// <param name="_binArray">バイナリ配列のポインタ</param>
		/// <param name="_binArraySize">バイナリ配列のサイズ</param>
		static void Write(const char* _binArray, const size_t& _binArraySize);

		/// <summary>
		/// ログ一覧を生成する
		/// </summary>
		/// <returns></returns>
		static std::string GenerateLogs();
		/// <summary>
		/// ログが発行されたときの処理
		/// </summary>
		/// <param name="_callback">コールバック関数(文字列)</param>
		static void OnLog(const OnLogFunc& _callback);

	private:
		Log() = delete;
		~Log() = delete;

	private:
		static std::list<std::string> logs_;  // ログ貯めておく
		static OnLogFunc onLogFunc_;  // ログが発行されたときのコールバック関数
		static std::mutex logWriteMutex_;  // ログを書き出す排他制御用
	};
}
