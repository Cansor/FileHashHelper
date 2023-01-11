// ����������� DialogBox

#pragma once
#include <Windows.h>

class MainDialog {
  public:
    /// <summary>
    /// ���캯��
    /// </summary>
    /// <param name="templateID">�Ի������ԴID</param>
    /// <param name="hInstance">����ʵ���ľ��</param>
    MainDialog(DWORD templateID, HINSTANCE hInstance);

    // ��ʾ�Ի���
    void Show();

    // ���ı��༭��׷���ı�
    void EditAppendText(LPCTSTR text) const;

  private:
    /// <summary>
    /// ��Ϣ������
    /// </summary>
    /// <param name="hwndDlg">��ǰ���ڵľ��</param>
    /// <param name="uMsg">�¼���Ϣ</param>
    /// <param name="wParam">�¼�����</param>
    /// <param name="lParam"></param>
    /// <returns>���� true ��ʾ��Ϣ�Ѵ������� false �򽻸�ϵͳ����</returns>
    INT_PTR DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // �Ի���ĳ�ʼ��
    void OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam);

    // ����ļ���ѡ����ҪУ����ļ�
    void OnOpen(HWND hwndDlg, UINT event);
    // ����У����
    void OnSave(HWND hwndDlg, UINT event) const;
    // ����У������������
    void OnCopy(HWND hwndDlg, UINT event) const;
    // ����༭���е��ı�
    void OnClear(HWND hwndDlg, UINT event) const;
    // ֹͣ����ִ�е�����
    void OnStop(HWND hwndDlg, UINT event);

    // ��ק�ļ�������ʱ����ʼ�ļ�У��
    void OnDropFiles(HWND hwndDlg, WPARAM wParam, LPARAM lParam);

    // �����ڡ��˵�����¼�����
    void OnMenuAbout(HWND hwndDlg) const;

    // ��ȡ���ò���
    void GetParams(HWND hwndDlg);

  public:
    HWND progressBar;
    HWND editBox;
    HWND btnOpen;
    HWND btnSave;
    HWND btnCopy;
    HWND btnClear;
    HWND btnStop;

    BOOL isRun;
    BOOL md5;
    BOOL crc32;
    BOOL crc64;
    BOOL sha1;
    BOOL isUpper;
    int proBarMaxRange;

  private:
    DWORD templateID;
    HINSTANCE hInstance;
    HWND hDialog;
};
