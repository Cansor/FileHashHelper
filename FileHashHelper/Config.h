/*
 * ���õĶ�ȡ��д��
 */

#pragma once
#include <Shlobj.h>

class ConfigReg {
  public:
    ConfigReg();

    /**
     * \brief �������һ��ע�����ڵ���������Ա����ǰ�����ȵ��ñ�������һ��ע����
     * \param hKye ע�����ľ����������Ԥ������֮һ���� HKEY_CURRENT_USER
     * \param lpSubKey ���������
     * \param samDesired ����Ȩ�޵����롣�� KEY_READ, KEY_SET_VALUE
     * \return �ɹ����� ERROR_SUCCESS, ʧ�ܷ��ط� 0 �������
     */
    LSTATUS CreateKey(HKEY hKye, LPCTSTR lpSubKey, REGSAM samDesired);

    /**
     * \brief ��ָ����ע���������������
     * \param valueName ��������
     * \param dwType ���ݵ����͡��� REG_SZ(�ַ���)��REG_DWORD(32λ������)��REG_BINARY(����������)��REG_NONE(δ����ֵ����)
     * \param data Ҫ�洢�����ݣ�������ַ���������� null ��β
     * \param len ���ݵĳ��ȡ�������ַ������������β null ��ֹ��
     * \return �ɹ����� ERROR_SUCCESS, ʧ�ܷ��ط� 0 �������
     */
    LSTATUS SetValue(const wchar_t* valueName, DWORD dwType, const BYTE* data, DWORD len) const;

    /**
     * \brief ��ȡָ��ע����������
     * \param valueName ��������
     * \param pvData �������ݵĻ�����
     * \param dataLen ָ��һ���������ͱ�����ָ�룬�ñ���ָʾд�뵽 pvData �����ݴ�С��ֻ�е� pvData Ϊ��ʱ����Ϊ�ա�
     * \return �ɹ����� ERROR_SUCCESS, ʧ�ܷ��ط� 0 �������
     */
    LSTATUS GetValue(const wchar_t* valueName, PVOID pvData, LPDWORD dataLen) const;

    /**
     * \brief ɾ��ָ����ע����Ҫɾ�������������
     * \param samDesired �ض�ƽ̨�ķ������룬KEY_WOW64_32KEY(32λϵͳ) �� KEY_WOW64_64KEY(64λϵͳ)
     * \return �ɹ����� ERROR_SUCCESS, ʧ�ܷ��ط� 0 �������
     */
    LSTATUS DeleteKey(REGSAM samDesired) const;

    /**
     * \brief ��ע�����ɾ��ָ�������ݣ�ע�����ľ��������ʹ�� KEY_SET_VALUE ����Ȩ�޴򿪵�
     * \param valueName ��������
     * \return �ɹ����� ERROR_SUCCESS, ʧ�ܷ��ط� 0 �������
     */
    LSTATUS DeleteValue(const wchar_t* valueName) const;

    /**
     * \brief �ر�ע�������������򿪵Ĳ���Ԥ����ע�����֮һ������Ҫ���ô˺����رվ��
     * \return �ɹ����� ERROR_SUCCESS, ʧ�ܷ��ط� 0 �������
     */
    LSTATUS Close() const;

  private:
    HKEY openKey;
    HKEY hKey;
    LPCTSTR subKay;
};
