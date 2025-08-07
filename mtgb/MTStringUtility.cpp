#include "MTStringUtility.h"
#include "IncludingWindows.h"
#include <codecvt>

std::wstring mtgb::MultiToWide(const std::string& _string)
{
    int wlen = MultiByteToWideChar(
        CP_ACP,//変換に使用するコードページを指定(システムの既定コードページを使用)
        0,//変換の種類
        _string.c_str(),//変換する文字列
        -1,//文字列のサイズ。-1の場合終端のnull文字を含む文字列全体
        NULL, //変換された文字列を受け取る
        0);//文字列を受け取るバッファのサイズ,-1の場合文字列全体

    std::wstring wstr(wlen, L'\0');
    MultiByteToWideChar(
        CP_ACP, 
        0, 
        _string.c_str(), 
        -1, 
        &wstr[0], 
        wlen);

    return wstr;
}

std::string mtgb::WideToMulti(const std::wstring& _wstring)
{
    int len = WideCharToMultiByte(
        CP_ACP,
        0,
        _wstring.c_str(),
        -1,
        NULL,
        0,
        NULL,
        NULL);

    std::string str(len, '\0');

    WideCharToMultiByte(
        CP_ACP,
        0,
        _wstring.c_str(),
        -1,
        &str[0],
        len,
        NULL,
        NULL);

    return str;
}

std::wstring mtgb::UTF8ToWide(const std::string& _string)
{
    int wlen = MultiByteToWideChar(
        CP_UTF8,
        0,
        _string.c_str(),
        -1,
        NULL,
        0);

    std::wstring wstr(wlen, '\0');
    MultiByteToWideChar(
        CP_UTF8,
        0,
        _string.c_str(),
        -1,
        &wstr[0],
        wlen);
    
    return wstr;
}

std::string mtgb::WideToUTF8(const std::wstring& _wstring)
{
    //utf16→utf8
    int u8len = WideCharToMultiByte(
        CP_UTF8,
        0,
        _wstring.c_str(),
        -1,
        NULL, 
        0, 
        NULL, 
        NULL);

    std::string utf8str(u8len, '\0');

    WideCharToMultiByte(
        CP_UTF8,
        0,
        _wstring.c_str(),//変換する文字列
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

std::string mtgb::MultiToUTF8(const std::string& _sjisStr)
{
    //Shitf-JIS → UTF-16 → UTF-8
    return WideToUTF8(MultiToWide(_sjisStr));
}

std::string mtgb::UTF8ToMulti(const std::string& _utf8Str)
{
    return WideToMulti(UTF8ToWide(_utf8Str));
}
