// �ļ��������

#pragma once
#include <Shlobj.h>
#include <vector>

/// <summary>
/// ���ļ�ѡ��Ի���
/// 
/// ע�⣬vector �����е���Դ����ֱ��������ڴ�֮ǰ����
/// ���� CoTaskMemFree �����ͷ��ڴ档
/// </summary>
/// <param name="vec">���ڱ����ļ�·��������</param>
/// <param name="multiSelect">true ʱ�����ѡ</param>
/// <param name="fileType">�ļ����͹���������</param>
/// <param name="fileTypeLen">����ĳ���</param>
/// <param name="fileTypeIndex">Ĭ��ѡ��Ĺ�����������</param>
void SelectFileDialog(std::vector<LPWSTR> &vec, BOOL multiSelect = FALSE, COMDLG_FILTERSPEC* fileType = nullptr,
    UINT fileTypeLen = 1, UINT fileTypeIndex = 1);

/**
 * \brief �򿪱����ļ��Ի���
 *        ע�⣬���صĽ������� CoTaskMemFree �����ͷ��ڴ档
 * \param fileName Ĭ�ϱ�����ļ���
 * \param fileType �ļ����͹���������
 * \param fileTypeLen ����ĳ���
 * \param fileTypeIndex Ĭ��ѡ��Ĺ�����������
 * \return ѡ����ļ�ȫ·��
 */
LPWSTR SaveFileDialog(LPCTSTR fileName = nullptr, COMDLG_FILTERSPEC* fileType = nullptr,
    UINT fileTypeLen = 1, UINT fileTypeIndex = 1);

/// <summary> ��ȡ�ļ���С</summary>
/// <param name="fileName">�ļ�ȫ·����</param>
/// <returns>�ļ��� Byte ��С�����ʧ���򷵻� -1</returns>
int64_t GetFileSize(LPWSTR fileName);