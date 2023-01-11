// 程序的主界面 DialogBox

#pragma once
#include <Windows.h>

class MainDialog {
  public:
    /// <summary>
    /// 构造函数
    /// </summary>
    /// <param name="templateID">对话框的资源ID</param>
    /// <param name="hInstance">程序实例的句柄</param>
    MainDialog(DWORD templateID, HINSTANCE hInstance);

    // 显示对话框
    void Show();

    // 向文本编辑框追加文本
    void EditAppendText(LPCTSTR text) const;

  private:
    /// <summary>
    /// 消息处理函数
    /// </summary>
    /// <param name="hwndDlg">当前窗口的句柄</param>
    /// <param name="uMsg">事件消息</param>
    /// <param name="wParam">事件参数</param>
    /// <param name="lParam"></param>
    /// <returns>返回 true 表示消息已处理，返回 false 则交给系统处理</returns>
    INT_PTR DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // 对话框的初始化
    void OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam);

    // 浏览文件，选择需要校验的文件
    void OnOpen(HWND hwndDlg, UINT event);
    // 保存校验结果
    void OnSave(HWND hwndDlg, UINT event) const;
    // 复制校验结果到剪贴版
    void OnCopy(HWND hwndDlg, UINT event) const;
    // 清除编辑框中的文本
    void OnClear(HWND hwndDlg, UINT event) const;
    // 停止正在执行的任务
    void OnStop(HWND hwndDlg, UINT event);

    // 拖拽文件到窗口时，开始文件校验
    void OnDropFiles(HWND hwndDlg, WPARAM wParam, LPARAM lParam);

    // “关于”菜单项的事件处理
    void OnMenuAbout(HWND hwndDlg) const;

    // 获取配置参数
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
