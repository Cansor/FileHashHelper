/*
 * 配置的读取与写入
 */

#pragma once
#include <Shlobj.h>

class ConfigReg {
  public:
    ConfigReg();

    /**
     * \brief 创建或打开一个注册表项。在调用其他成员函数前必须先调用本函数打开一个注册表项。
     * \param hKye 注册表项的句柄，可以是预定义项之一，如 HKEY_CURRENT_USER
     * \param lpSubKey 子项的名称
     * \param samDesired 访问权限的掩码。如 KEY_READ, KEY_SET_VALUE
     * \return 成功返回 ERROR_SUCCESS, 失败返回非 0 错误代码
     */
    LSTATUS CreateKey(HKEY hKye, LPCTSTR lpSubKey, REGSAM samDesired);

    /**
     * \brief 在指定的注册表项中设置数据
     * \param valueName 数据名称
     * \param dwType 数据的类型。如 REG_SZ(字符串)、REG_DWORD(32位的数字)、REG_BINARY(二进制数据)、REG_NONE(未定义值类型)
     * \param data 要存储的数据，如果是字符串则必需以 null 结尾
     * \param len 数据的长度。如果是字符串，则包括结尾 null 终止符
     * \return 成功返回 ERROR_SUCCESS, 失败返回非 0 错误代码
     */
    LSTATUS SetValue(const wchar_t* valueName, DWORD dwType, const BYTE* data, DWORD len) const;

    /**
     * \brief 获取指定注册表项的数据
     * \param valueName 数据名称
     * \param pvData 接收数据的缓冲区
     * \param dataLen 指向一个数字类型变量的指针，该变量指示写入到 pvData 的数据大小。只有当 pvData 为空时才能为空。
     * \return 成功返回 ERROR_SUCCESS, 失败返回非 0 错误代码
     */
    LSTATUS GetValue(const wchar_t* valueName, PVOID pvData, LPDWORD dataLen) const;

    /**
     * \brief 删除指定的注册表项。要删除的项不能有子项
     * \param samDesired 特定平台的访问掩码，KEY_WOW64_32KEY(32位系统) 或 KEY_WOW64_64KEY(64位系统)
     * \return 成功返回 ERROR_SUCCESS, 失败返回非 0 错误代码
     */
    LSTATUS DeleteKey(REGSAM samDesired) const;

    /**
     * \brief 从注册表项删除指定的数据，注册表项的句柄必须是使用 KEY_SET_VALUE 访问权限打开的
     * \param valueName 数据名称
     * \return 成功返回 ERROR_SUCCESS, 失败返回非 0 错误代码
     */
    LSTATUS DeleteValue(const wchar_t* valueName) const;

    /**
     * \brief 关闭注册表项句柄。如果打开的不是预定义注册表项之一，则需要调用此函数关闭句柄
     * \return 成功返回 ERROR_SUCCESS, 失败返回非 0 错误代码
     */
    LSTATUS Close() const;

  private:
    HKEY openKey;
    HKEY hKey;
    LPCTSTR subKay;
};
