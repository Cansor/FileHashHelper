/**
 * ��װ��һЩ���õĲ����ַ����ĺ���
 */

#pragma once
#include <iostream>

/// <summary>
/// long long ת char �ַ�����
/// �����ڲ������һ���̶� 20 �� char ��С��new char[20]�����ڴ�ռ�� buf��
/// ����������ֵ��Ϊ������ʹ�õ��Ĵ�С��
/// </summary>
/// <param name="buf">�ַ��������� null ָ�룬�����ڲ����Զ������ڴ棻֮�󣬵��������Լ� delete[] �ͷ��ڴ�</param>
/// <param name="number">Ҫת���ַ�������������</param>
/// <returns>�ɹ��򷵻�д�뻺�����ĳ��ȣ����� null ��ֹ����</returns>
//int LLongToChar(char** buf, long long number);

/// <summary>
/// long long ת wchar_t �ַ�����
/// </summary>
/// <param name="buf">�ַ��������� null ָ�룬�����ڲ����Զ������ڴ棻֮�󣬵��������Լ� delete[] �ͷ��ڴ�</param>
/// <param name="number">Ҫת���ַ�������������</param>
/// <returns>���� null ��ֹ�����ڵ��ַ�����</returns>
//int LLongToWchar(wchar_t** buf, long long number);

/// <summary>
/// string �����ַ���ת wchar_t ���͡�
/// </summary>
/// <param name="buf">�ַ��������� null ָ�룬�����ڲ����Զ������ڴ棻֮�󣬵��������Լ� delete[] �ͷ��ڴ�</param>
/// <param name="str">string �ַ���</param>
/// <returns>���� null ��ֹ�����ڵ��ַ�����</returns>
//size_t StringToWchar(wchar_t** buf, const std::string &str);

/// <summary>
/// char �����ַ���ת wchar_t ���͡�
/// </summary>
/// <param name="buf">
/// �ַ��������� null ָ�룬�����ڲ����Զ������ڴ棻
/// ֮�󣬵��������Լ� delete[] �ͷ��ڴ档
/// </param>
/// <param name="cstr">char* �ַ�������β������ null ��ֹ��</param>
/// <returns>���� null ��ֹ�����ڵ��ַ�����</returns>
size_t CharToWchar(wchar_t** buf, const char* cstr);

/**
 * \brief wchar_t �����ַ���ת char ���͡�
 * \param buf �ַ��������� null ָ�룬�����ڲ����Զ������ڴ棻
 *        ֮�󣬵��������Լ� delete[] �ͷ��ڴ档
 * \param wstr wchar_t* �ַ�������β������ null ��ֹ��
 * \return ���� null ��ֹ�����ڵ��ַ�����
 */
size_t WcharToChar(char** buf, const wchar_t* wstr);

/**
 * \brief ɾ�� string ������ƥ����Ӵ�
 * \param str ԭ�ַ���
 * \param substr ����ƥ���Ӵ�
 * \param count ��ƥ�䵽�Ӵ�ʱɾ�����ַ�����
 * \return �Ӵ����ֵĴ���
 */
int DeleteSubstringAll(std::string& str, const std::string& substr, int count);