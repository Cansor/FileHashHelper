/**
 * 封装了一些常用的操作字符串的函数
 */

#include "StringUtils.h"
#include "Config.h"


//int LLongToChar(char** buf, const long long number) {
//    *buf = new char[20];
//    const int len = sprintf_s(*buf, 20, "%lld", number);
//    return len < 0 ? len : len + 1;
//}

//int LLongToWchar(wchar_t** buf, const long long number) {
//    *buf = new wchar_t[20];
//    const int len = wsprintf(*buf, L"%d", number);
//    return len < 0 ? len : len + 1;
//}

//size_t StringToWchar(wchar_t** buf, const std::string& str) {
//    return CharToWchar(buf, str.c_str());
//}

size_t CharToWchar(wchar_t** buf, const char* cstr) {
    //const int len = MultiByteToWideChar(CP_ACP, 0, cstr, -1/*(int) strlen(cstr)*/, nullptr, 0);
    //*buf = new wchar_t[len];
    //return MultiByteToWideChar(CP_ACP, 0, cstr, -1/*(int) strlen(cstr)*/, *buf, len);

    const size_t len = strlen(cstr) + 1;
    *buf = new wchar_t[len];
    wsprintf(*buf, L"%S", cstr);
    return len;
}

size_t WcharToChar(char** buf, const wchar_t* wstr) {
    /*const int iSize = WideCharToMultiByte(CP_ACP, 0, wstr, -1, nullptr, 0,
    nullptr, nullptr); *buf = new char[iSize]; return
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, *buf, iSize, nullptr, nullptr);*/

    size_t converted = 0;
    size_t len = 0;
    // 使用系统默认的区域
    setlocale(LC_CTYPE, "");
    wcstombs_s(&len, nullptr, 0, wstr, 0);
    len++;
    *buf = new char[len];
    wcstombs_s(&converted, *buf, len, wstr, len - 1);

    return converted;
}

int DeleteSubstringAll(std::string& str, const std::string& substr, const int count) {
    int num = 0;
    size_t pos;
    while ((pos = str.find(substr)) != std::string::npos) {
      str.erase(pos, count);
        num++;
    }
    return num;
}