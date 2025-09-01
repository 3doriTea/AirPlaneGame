#pragma once
#include <string>
#include <string_view>
#include "MTAssert.h"

namespace mtgb
{
	/// <summary>
	/// UTF8 文字数(バイト数ではない)を取得する
	/// </summary>
	/// <param name="_string">utf8の文字列</param>
	/// <returns>文字数</returns>
	static inline size_t GetSizeUTF8Characters(const std::u8string& _string)
	{
		enum BYTE_SIZE : int
		{
			BYTE_SIZE_IDK,
			BYTE_SIZE1,
			BYTE_SIZE2,
			BYTE_SIZE3,
			BYTE_SIZE4,
		};

		static auto isMatchByteSize
		{
			[](const uint8_t _byte, const uint8_t _idByte, const uint8_t _byteMask) -> bool
			{
				return (_byte & _byteMask) == _idByte;
			}
		};
		
		// REF: https://ja.wikipedia.org/wiki/UTF-8
		size_t charasCount{};
		for (int byteIndex = 0; byteIndex < _string.size();)
		{
			BYTE_SIZE charaByteSize{};
			if (isMatchByteSize(_string[byteIndex], 0b0000'0000, 0b1000'0000))
			{
				charaByteSize = BYTE_SIZE1;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1100'0000, 0b1110'0000))
			{
				charaByteSize = BYTE_SIZE2;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1110'0000, 0b1111'0000))
			{
				charaByteSize = BYTE_SIZE3;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1111'0000, 0b1111'1000))
			{
				charaByteSize = BYTE_SIZE4;
			}

			massert(charaByteSize != BYTE_SIZE_IDK && "バイトカウントが不一致 @SubStrBegin");

			byteIndex += charaByteSize;
			charasCount++;
		}

		return charasCount;
	}

	/// <summary>
	/// utf8文字列を切り取る
	/// </summary>
	/// <param name="_string">utf8文字列</param>
	/// <param name="_count">文字数(バイト数ではない)</param>
	/// <returns>切り取られたutf8文字列</returns>
	static inline std::u8string SubStrBegin(const std::u8string& _string, const int _count)
	{
		enum BYTE_SIZE : int
		{
			BYTE_SIZE_IDK,
			BYTE_SIZE1,
			BYTE_SIZE2,
			BYTE_SIZE3,
			BYTE_SIZE4,
		};

		int charaCount{};  // utf8の文字数カウント
		int byteCount{};  // 1文字を構成するバイト数カウント
		int byteIndex{};

		static auto isMatchByteSize
		{
			[](const uint8_t _byte, const uint8_t _idByte, const uint8_t _byteMask) -> bool
			{
				return (_byte & _byteMask) == _idByte;
			}
		};

		// REF: https://ja.wikipedia.org/wiki/UTF-8
		for (byteIndex = 0; charaCount < _count && byteIndex < _string.size();)
		{
			BYTE_SIZE charaByteSize{};
			if (isMatchByteSize(_string[byteIndex], 0b0000'0000, 0b1000'0000))
			{
				charaByteSize = BYTE_SIZE1;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1100'0000, 0b1110'0000))
			{
				charaByteSize = BYTE_SIZE2;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1110'0000, 0b1111'0000))
			{
				charaByteSize = BYTE_SIZE3;
			}
			else if (isMatchByteSize(_string[byteIndex], 0b1111'0000, 0b1111'1000))
			{
				charaByteSize = BYTE_SIZE4;
			}

			massert(charaByteSize != BYTE_SIZE_IDK && "バイトカウントが不一致 @SubStrBegin");

			byteIndex += charaByteSize;
			charaCount++;
		}

		return _string.substr(0, byteIndex);
	}

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
