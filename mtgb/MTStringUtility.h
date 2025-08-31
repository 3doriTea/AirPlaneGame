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

	/// <summary>
	/// マルチバイト文字列からワイド文字列へ変換
	/// </summary>
	/// <param name="_string">変換する文字列</param>
	/// <returns></returns>
	std::wstring MultiToWide(const std::string& _string);
	/// <summary>
	/// マルチバイト文字列からワイド文字列へ変換
	/// </summary>
	/// <param name="_string">文字列</param>
	/// <returns></returns>
	std::wstring MultiToWide(const char* _string);

	std::string WideToMulti(const std::wstring& _wstring);
	std::string WideToMulti(const wchar_t* _wstring);

	/// <summary>
	/// マルチバイト文字列からワイド文字列へ変換
	/// </summary>
	/// <param name="_string">変換する文字列</param>
	/// <returns></returns>
	std::string MultiToUTF8(const std::string & _string);
	/// <summary>
	/// マルチバイト文字列からワイド文字列へ変換
	/// </summary>
	/// <param name="_string">変換する文字列</param>
	/// <returns></returns>
	std::string MultiToUTF8(const char* _string);

	std::wstring UTF8ToWide(const std::string& _string);
	std::wstring UTF8ToWide(const std::u8string& _string);
	std::wstring UTF8ToWide(const char* _string);

	std::string WideToUTF8(const std::wstring& _wstring);
	std::string WideToUTF8(const wchar_t* _wstring);

	std::string UTF8ToMulti(const std::string & _string);
	std::string UTF8ToMulti(const char * _string);
}
