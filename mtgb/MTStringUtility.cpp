#include "MTStringUtility.h"
#include "IncludingWindows.h"
#include "MTAssert.h"

std::wstring mtgb::MultiToWide(const std::string& _string)
{
	return MultiToWide(_string.c_str());
}

std::wstring mtgb::MultiToWide(const char* _string)
{
	int wlen = MultiByteToWideChar(
		CP_ACP,//変換に使用するコードページを指定(システムの既定コードページを使用)
		0,//変換の種類
		_string,//変換する文字列
		-1,//文字列のサイズ。-1の場合終端のnull文字を含む文字列全体
		NULL, //変換された文字列を受け取る
		0);//文字列を受け取るバッファのサイズ,-1の場合文字列全体

	std::wstring wstr(wlen, L'\0');
	MultiByteToWideChar(
		CP_ACP,
		0,
		_string,
		-1,
		&wstr[0],
		wlen);

	return wstr;
}

std::string mtgb::WideToMulti(const std::wstring& _wstring)
{
	return WideToMulti(_wstring.c_str());
}

std::string mtgb::WideToMulti(const wchar_t* _wstring)
{
	int len = WideCharToMultiByte(
		CP_ACP,
		0,
		_wstring,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	std::string str(len, '\0');

	WideCharToMultiByte(
		CP_ACP,
		0,
		_wstring,
		-1,
		&str[0],
		len,
		NULL,
		NULL);

	if (!str.empty() && str.back() == '\0')
	{
		str.pop_back();
	}

	return str;
}

std::wstring mtgb::UTF8ToWide(const std::string& _string)
{
	return UTF8ToWide(_string.c_str());
}

std::wstring mtgb::UTF8ToWide(const std::u8string& _string)
{
	if (_string.empty()) return std::wstring();

	// 必要なバッファサイズを取得
	int size = MultiByteToWideChar(
		CP_UTF8,  // ソースのコードページ (UTF-8)
		MB_ERR_INVALID_CHARS,
		reinterpret_cast<const char*>(_string.c_str()),
		static_cast<int>(_string.size()),
		nullptr,  // 出力先バッファなし
		0         // バッファサイズ取得
	);

	massert(size != 0 && "Invalid UTF-8 sequence");

	// 変換用バッファ確保
	std::wstring wstr(size, L'\0');
	int ret = MultiByteToWideChar(
		CP_UTF8,
		MB_ERR_INVALID_CHARS,
		reinterpret_cast<const char*>(_string.c_str()),
		static_cast<int>(_string.size()),
		wstr.data(),
		size
	);

	massert(ret != 0 && "Invalid UTF-8 sequence");

	return wstr;
}

std::wstring mtgb::UTF8ToWide(const char* _string)
{
	int wlen = MultiByteToWideChar(
		CP_UTF8,
		0,
		_string,
		-1,
		NULL,
		0);

	std::wstring wstr(wlen, '\0');
	MultiByteToWideChar(
		CP_UTF8,
		0,
		_string,
		-1,
		&wstr[0],
		wlen);

	return wstr;
}

std::string mtgb::WideToUTF8(const std::wstring& _wstring)
{
	return WideToUTF8(_wstring.c_str());
}

std::string mtgb::WideToUTF8(const wchar_t* _wstring)
{
	//utf16→utf8
	int u8len = WideCharToMultiByte(
		CP_UTF8,
		0,
		_wstring,
		-1,
		NULL,
		0,
		NULL,
		NULL);

	std::string utf8str(u8len, '\0');

	WideCharToMultiByte(
		CP_UTF8,
		0,
		_wstring,//変換する文字列
		-1,//文字列のサイズ。終端がnull文字の場合-1
		&utf8str[0],//変換された文字列を受け取る
		u8len, //文字列を受け取るバッファのサイズ
		NULL, //指定したコードページで表すことができない場合に使用する文字へのポインタ
		NULL);//表すことができずに既定の文字(?)を使用したかどうかを示すフラグへのポインタ

	//末尾の'\0'を除去
	if (!utf8str.empty() && utf8str.back() == '\0')
	{
		utf8str.pop_back();
	}

	return utf8str;
}

std::string mtgb::MultiToUTF8(const std::string& _string)
{
	//Shitf-JIS → UTF-16 → UTF-8
	return WideToUTF8(MultiToWide(_string));
}

std::string mtgb::MultiToUTF8(const char* _string)
{
	return WideToUTF8(MultiToWide(_string));
}

std::string mtgb::UTF8ToMulti(const std::string& _string)
{
	return WideToMulti(UTF8ToWide(_string));
}

std::string mtgb::UTF8ToMulti(const char* _string)
{
	return WideToMulti(UTF8ToWide(_string));
}
