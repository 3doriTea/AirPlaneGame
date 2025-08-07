#include "MTStringUtility.h"
#include "IncludingWindows.h"
#include <codecvt>

std::wstring mtgb::MultiToWide(const std::string& _string)
{
    int wlen = MultiByteToWideChar(
        CP_ACP,//�ϊ��Ɏg�p����R�[�h�y�[�W���w��(�V�X�e���̊���R�[�h�y�[�W���g�p)
        0,//�ϊ��̎��
        _string.c_str(),//�ϊ����镶����
        -1,//������̃T�C�Y�B-1�̏ꍇ�I�[��null�������܂ޕ�����S��
        NULL, //�ϊ����ꂽ��������󂯎��
        0);//��������󂯎��o�b�t�@�̃T�C�Y,-1�̏ꍇ������S��

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
    //utf16��utf8
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
        _wstring.c_str(),//�ϊ����镶����
        -1,//������̃T�C�Y�B�I�[��null�����̏ꍇ-1
        &utf8str[0],//�ϊ����ꂽ��������󂯎��
        u8len, //��������󂯎��o�b�t�@�̃T�C�Y
        NULL, //�w�肵���R�[�h�y�[�W�ŕ\�����Ƃ��ł��Ȃ��ꍇ�Ɏg�p���镶���ւ̃|�C���^
        NULL);//�\�����Ƃ��ł����Ɋ���̕���(?)���g�p�������ǂ����������t���O�ւ̃|�C���^
    
    //������'\0'������
    if (!utf8str.empty() && utf8str.back() == '\0')
    {
        utf8str.pop_back();
    }

    return utf8str;
}

std::string mtgb::MultiToUTF8(const std::string& _sjisStr)
{
    //Shitf-JIS �� UTF-16 �� UTF-8
    return WideToUTF8(MultiToWide(_sjisStr));
}

std::string mtgb::UTF8ToMulti(const std::string& _utf8Str)
{
    return WideToMulti(UTF8ToWide(_utf8Str));
}
