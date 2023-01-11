/**
 * 封装了一些常用的操作字符串的函数
 */

#pragma once
#include <iostream>

/// <summary>
/// long long 转 char 字符串。
/// 函数内部会分配一个固定 20 个 char 大小（new char[20]）的内存空间给 buf，
/// 而函数返回值则为其真正使用到的大小。
/// </summary>
/// <param name="buf">字符缓冲区的 null 指针，函数内部会自动分配内存；之后，调用者需自己 delete[] 释放内存</param>
/// <param name="number">要转成字符串的整型数字</param>
/// <returns>成功则返回写入缓冲区的长度（包括 null 终止符）</returns>
//int LLongToChar(char** buf, long long number);

/// <summary>
/// long long 转 wchar_t 字符串。
/// </summary>
/// <param name="buf">字符缓冲区的 null 指针，函数内部会自动分配内存；之后，调用者需自己 delete[] 释放内存</param>
/// <param name="number">要转成字符串的整型数字</param>
/// <returns>包含 null 终止符在内的字符长度</returns>
//int LLongToWchar(wchar_t** buf, long long number);

/// <summary>
/// string 类型字符串转 wchar_t 类型。
/// </summary>
/// <param name="buf">字符缓冲区的 null 指针，函数内部会自动分配内存；之后，调用者需自己 delete[] 释放内存</param>
/// <param name="str">string 字符串</param>
/// <returns>包含 null 终止符在内的字符长度</returns>
//size_t StringToWchar(wchar_t** buf, const std::string &str);

/// <summary>
/// char 类型字符串转 wchar_t 类型。
/// </summary>
/// <param name="buf">
/// 字符缓冲区的 null 指针，函数内部会自动分配内存；
/// 之后，调用者需自己 delete[] 释放内存。
/// </param>
/// <param name="cstr">char* 字符串，结尾必需是 null 终止符</param>
/// <returns>包含 null 终止符在内的字符长度</returns>
size_t CharToWchar(wchar_t** buf, const char* cstr);

/**
 * \brief wchar_t 类型字符串转 char 类型。
 * \param buf 字符缓冲区的 null 指针，函数内部会自动分配内存；
 *        之后，调用者需自己 delete[] 释放内存。
 * \param wstr wchar_t* 字符串，结尾必需是 null 终止符
 * \return 包含 null 终止符在内的字符长度
 */
size_t WcharToChar(char** buf, const wchar_t* wstr);

/**
 * \brief 删除 string 中所有匹配的子串
 * \param str 原字符串
 * \param substr 用于匹配子串
 * \param count 当匹配到子串时删除的字符个数
 * \return 子串出现的次数
 */
int DeleteSubstringAll(std::string& str, const std::string& substr, int count);