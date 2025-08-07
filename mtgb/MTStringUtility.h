#pragma once
#include <string>
#include <string_view>

namespace mtgb
{
	/// <summary>
	/// ワイド文字列に変換する
	/// </summary>
	/// <param name="_cstr"></param>
	/// <returns></returns>
	/*static inline std::wstring ToWString(char* _cstr)
	{
		std::string str{ _cstr };
		return ToWString(str);
	}*/

	/// <summary>
	/// ワイド文字列に変換する
	/// </summary>
	/// <param name="_string"></param>
	/// <returns></returns>
	static inline std::wstring ToWString(const std::string& _string)
	{
		return { _string.begin(), _string.end() };
	}

	/// <summary>
	/// ワイド文字列に変換する
	/// </summary>
	/// <param name="_stringView"></param>
	/// <returns></returns>
	static inline std::wstring ToWString(const std::string_view& _stringView)
	{
		return { _stringView.begin(), _stringView.end() };
	}

	static std::wstring MultiToWide(const std::string& _string);
	static std::string WideToMulti(const std::wstring& _wstring);

	static std::string MultiToUTF8(const std::string& _sjisStr);

	static std::wstring UTF8ToWide(const std::string& _string);
	static std::string WideToUTF8(const std::wstring& _wstring);
	static std::string UTF8ToMulti(const std::string& _utf8Str);
}
