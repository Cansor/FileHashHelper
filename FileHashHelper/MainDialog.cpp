// 主对话框的实现

#include "targetver.h"
#include "MainDialog.h"
#include "Resource.h"
#include <Richedit.h>
#include <process.h>
#include "FileUtils.h"
#include "StringUtils.h"
#include "md5.h"
#include "crc.h"
#include "sha1.hpp"
#include "Config.h"


constexpr const wchar_t* VALUE_NAME_MD5 = L"md5";
constexpr const wchar_t* VALUE_NAME_CRC32 = L"crc32";
constexpr const wchar_t* VALUE_NAME_CRC64 = L"crc64";
constexpr const wchar_t* VALUE_NAME_SHA1 = L"sha1";
constexpr const wchar_t* VALUE_NAME_UPPERCASE = L"upperCase";
constexpr const wchar_t* REG_SUB_KEY = L"Software\\iSpellBook\\FileHashHelper";

// FilePathVecFree 函数辅助标记
// true 使用 CoTaskMemFree
// false 使用 delete[]
BOOL gFreeType = FALSE;

MainDialog* gThatApp = nullptr;
// 存放文件的路径
std::vector<LPWSTR> gFilePathVec;

// 释放 gFilePathVec 容器中的资源
void FilePathVecFree() {
    for (const auto value : gFilePathVec) {
        if (gFreeType) CoTaskMemFree(value);
        else delete[] value;
    }
    gFilePathVec.clear();
}

// 把 hash 结果输出到文本编辑框
void OutputHashText(MD5& md5, crc::CRC32& crc32, crc::XzCRC64& crc64, SHA1& sha1) {
    if (gThatApp->md5) {
        const std::wstring md5Str = L"MD5:\t" + md5.toStringW(gThatApp->isUpper) + L"\r\n";
        gThatApp->EditAppendText(md5Str.c_str());
    }
    if (gThatApp->crc32) {
        const std::wstring crc32Str = L"CRC32:\t" + crc32.FinishW(gThatApp->isUpper) + L"\r\n";
        gThatApp->EditAppendText(crc32Str.c_str());
    }
    if (gThatApp->crc64) {
        std::wstring crc64Str = L"CRC64:\t" + crc64.FinishW(gThatApp->isUpper) + L"\r\n";
        gThatApp->EditAppendText(crc64Str.c_str());
    }
    if (gThatApp->sha1) {
        const std::wstring sha1Str = L"SHA1:\t" + sha1.finalW(gThatApp->isUpper) + L"\r\n";
        gThatApp->EditAppendText(sha1Str.c_str());
    }
    gThatApp->EditAppendText(L"\r\n");
}

// 计算文件的 Hash 值
void FilesHash() {
    if (gFilePathVec.empty()) return;

    MD5* md5 = new MD5();
    crc::CRC32* crc32 = new crc::CRC32();
    crc::XzCRC64* crc64 = new crc::XzCRC64();
    SHA1* sha1 = new SHA1();

    size_t fileSize = 0,
    count = 0;
    constexpr int buffSize = 8192;
    char* buffer = new char[buffSize];
    memset(buffer, 0, buffSize);

    // 获取所有文件的总大小
    for (const LPWSTR lpszFileName : gFilePathVec) {
        fileSize += GetFileSize(lpszFileName);
    }

    SendMessage(gThatApp->progressBar, PBM_SETPOS, 0, 0); // 重置进度条
    const size_t limit = fileSize / gThatApp->proBarMaxRange;

    // 遍历文件计算 hash 值
    for (const LPWSTR lpszFileName : gFilePathVec) {
        // 显示文件名称和大小
        gThatApp->EditAppendText(L"文件:\t");
        gThatApp->EditAppendText(lpszFileName);
        gThatApp->EditAppendText(L"\r\n");
        fileSize = GetFileSize(lpszFileName);

        wchar_t* fileSizeStrW = new wchar_t[20];
        wsprintf(fileSizeStrW, L"%d", fileSize);
        gThatApp->EditAppendText(L"大小:\t");
        gThatApp->EditAppendText(fileSizeStrW);
        gThatApp->EditAppendText(L" 字节\r\n");
        delete[] fileSizeStrW;

        // 如果未勾选任何计算项，就跳过
        if (!gThatApp->md5 && !gThatApp->crc32 && !gThatApp->crc64 && !gThatApp->sha1) {
            gThatApp->EditAppendText(L"\r\n");
            continue;
        }

        // 在计算前必需 reset.（sha1.hpp 在 final() 时已自动 reset）
        md5->reset();
        crc32->Reset();
        crc64->Reset();

        ifstream ifs(lpszFileName, std::ios::in | std::ios::binary);
        if (!ifs.is_open()) continue;

        std::streamsize len;

        // 核心部分，文件的 hash 值在此调用各算法计算
        while (!ifs.eof()) {
            ifs.read(buffer, buffSize);
            len = ifs.gcount();

            if (gThatApp->md5) md5->update(reinterpret_cast<const byte*>(buffer), len);
            if (gThatApp->crc32) crc32->Update(reinterpret_cast<crc::Byte*>(buffer), len);
            if (gThatApp->crc64) crc64->Update(reinterpret_cast<crc::Byte*>(buffer), len);
            if (gThatApp->sha1) sha1->update(buffer, len);

            count += len;
            if (count >= limit) {
                // 步进进度条
                SendMessage(gThatApp->progressBar, PBM_STEPIT, 0, 0);
                count = 0;
            }
            if (!gThatApp->isRun) {
                // 结束线程
                //_endthread();

                ifs.close();
                // 虽说不建议使用 goto, 不过在这里是比较简洁的方式了
                // 如果不用 goto, 也可以用两次 break 跳出这两层循环
                goto Terminate;
            }
        }

        ifs.close();
        OutputHashText(*md5, *crc32, *crc64, *sha1);
    }

  Terminate:
    if (gThatApp->isRun)
        SendMessage(gThatApp->progressBar, PBM_SETPOS, gThatApp->proBarMaxRange, 0);
    else
        gThatApp->EditAppendText(L"任务终止……\r\n\r\n");

    delete[] buffer;
    delete md5;
    delete crc32;
    delete crc64;
    delete sha1;
}

// 开启一个新线程，并在新线程中计算文件 Hash 值
void StartHashThread() {
    auto callbackFun = [](void*) {
        gThatApp->isRun = TRUE;
        // 开始任务时设置按钮相应的状态
        EnableWindow(gThatApp->btnOpen, FALSE);
        EnableWindow(gThatApp->btnStop, TRUE);
        // 执行
        FilesHash();
        // 任务完成后设置按钮相应的状态
        EnableWindow(gThatApp->btnOpen, TRUE);
        EnableWindow(gThatApp->btnSave, TRUE);
        EnableWindow(gThatApp->btnCopy, TRUE);
        EnableWindow(gThatApp->btnClear, TRUE);
        EnableWindow(gThatApp->btnStop, FALSE);
        // 清空文件路径
        FilePathVecFree();
        gThatApp->isRun = FALSE;
    };
    // 创建新线程
    // 在子线程中计算文件 Hash 值，避免阻塞主线程造成界面“卡死”
    _beginthread(callbackFun, 0, nullptr);
}

// 读取配置
void ReadConfig() {
    ConfigReg reg;
    LSTATUS status = reg.CreateKey(HKEY_CURRENT_USER, REG_SUB_KEY, KEY_READ);
    if (status != ERROR_SUCCESS)  return;

    BOOL* val = new BOOL(TRUE);
    LPDWORD len = reinterpret_cast<LPDWORD>(new LPDWORD);

    status = reg.GetValue(VALUE_NAME_MD5, val, len);
    if (status == ERROR_SUCCESS)  gThatApp->md5 = *val;
    status = reg.GetValue(VALUE_NAME_CRC32, val, len);
    if (status == ERROR_SUCCESS)  gThatApp->crc32 = *val;
    status = reg.GetValue(VALUE_NAME_CRC64, val, len);
    if (status == ERROR_SUCCESS)  gThatApp->crc64 = *val;
    status = reg.GetValue(VALUE_NAME_SHA1, val, len);
    if (status == ERROR_SUCCESS)  gThatApp->sha1 = *val;
    status = reg.GetValue(VALUE_NAME_UPPERCASE, val, len);
    if (status == ERROR_SUCCESS)  gThatApp->isUpper = *val;

    reg.Close();
    delete val;
    delete len;
}

// 保存配置
void WriteConfig() {
    ConfigReg reg;
    LSTATUS status = reg.CreateKey(HKEY_CURRENT_USER, REG_SUB_KEY, KEY_SET_VALUE);
    if (status != ERROR_SUCCESS) return;

    constexpr int len = sizeof(BOOL);

    reg.SetValue(VALUE_NAME_MD5, REG_DWORD, reinterpret_cast<BYTE*>(&gThatApp->md5), len);
    reg.SetValue(VALUE_NAME_CRC32, REG_DWORD, reinterpret_cast<BYTE*>(&gThatApp->crc32), len);
    reg.SetValue(VALUE_NAME_CRC64, REG_DWORD, reinterpret_cast<BYTE*>(&gThatApp->crc64), len);
    reg.SetValue(VALUE_NAME_SHA1, REG_DWORD, reinterpret_cast<BYTE*>(&gThatApp->sha1), len);
    reg.SetValue(VALUE_NAME_UPPERCASE, REG_DWORD, reinterpret_cast<BYTE*>(&gThatApp->isUpper), len);

    reg.Close();
}


MainDialog::MainDialog(DWORD templateID, HINSTANCE hInstance) {
    gThatApp = this;
    this->templateID = templateID;
    this->hInstance = hInstance;
    this->hDialog = nullptr;
    this->progressBar = nullptr;
    this->editBox = nullptr;
    this->btnOpen = nullptr;
    this->btnSave = nullptr;
    this->btnCopy = nullptr;
    this->btnClear = nullptr;
    this->btnStop = nullptr;
    this->isRun = FALSE;
    this->md5 = TRUE;
    this->crc32 = TRUE;
    this->crc64 = FALSE;
    this->sha1 = FALSE;
    this->isUpper = FALSE;
    this->proBarMaxRange = 1000;
}

void MainDialog::Show() {
    // CreateDialog 函数的最后一个参数——消息处理函数（又称窗口过程）不能是类的成员函数，
    // 所以通过这个“替身”来间接调用 MainDialog 的成员函数 DialogProc
    auto proc = [](HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        return gThatApp->DialogProc(hwndDlg, uMsg, wParam, lParam);
    };

    // 创建一个非模态对话框
    this->hDialog = CreateDialog(this->hInstance, MAKEINTRESOURCE(this->templateID), NULL, proc);
    // 显示对话框
    if (hDialog) ShowWindow(this->hDialog, SW_SHOW);

    MSG msg;

    // 消息循环
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!IsDialogMessage(this->hDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

}

// 窗口过程，消息处理
// 用户的所有操作都会通过系统消息发送，在这个函数中捕获消息系统并作出相应处理
INT_PTR MainDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG:
          OnInitDialog(hwndDlg, wParam, lParam);
          break;
        case WM_COMMAND: {
            const UINT controlId = LOWORD(wParam);  // 控件ID
            const UINT event = HIWORD(wParam);      // 事件类型
            switch (controlId) {
                case IDC_BTN_OPEN:
                    OnOpen(hwndDlg, event);
                    break;
                case IDC_BTN_SAVE:
                    OnSave(hwndDlg, event);
                    break;
                case IDC_BTN_COPY:
                    OnCopy(hwndDlg, event);
                    break;
                case IDC_BTN_CLEAR:
                    OnClear(hwndDlg, event);
                    break;
                case IDC_BTN_STOP:
                    OnStop(hwndDlg, event);
                    break;
                default:
                    break;
            }
        }
           break;
        case WM_DROPFILES:
            OnDropFiles(hwndDlg, wParam, lParam);
            break;
        case WM_SYSCOMMAND:
            if (wParam == IDM_ABOUT)
                OnMenuAbout(hwndDlg);
            return FALSE;
        case WM_CLOSE:
            this->GetParams(hwndDlg);
            WriteConfig();
            DestroyWindow(this->hDialog);
            return FALSE;
        case WM_DESTROY:
            PostQuitMessage(0);
            return FALSE;
        default:
            return FALSE;
    }

    return TRUE;
}

// 对话框创建完成后，进行初始化
void MainDialog::OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam) {
     // 设置窗口图标
    SendMessage(hwndDlg, WM_SETICON, ICON_SMALL,
        (LPARAM) LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON_SMALL)) );
    SendMessage(hwndDlg, WM_SETICON, ICON_BIG,
        (LPARAM) LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON_SMALL)) );
    
    // 添加系统菜单项
    const HMENU hMenu = GetSystemMenu(hwndDlg, FALSE);
    AppendMenu(hMenu, MF_STRING , IDM_ABOUT, L"关于(&A)");

    // 获取控件的句柄
    this->progressBar = GetDlgItem(hwndDlg, IDC_PROGRESS_1);
    this->editBox = GetDlgItem(hwndDlg, IDC_RICHEDIT_RESULT);
    this->btnOpen = GetDlgItem(hwndDlg, IDC_BTN_OPEN);
    this->btnSave = GetDlgItem(hwndDlg, IDC_BTN_SAVE);
    this->btnCopy = GetDlgItem(hwndDlg, IDC_BTN_COPY);
    this->btnClear = GetDlgItem(hwndDlg, IDC_BTN_CLEAR);
    this->btnStop = GetDlgItem(hwndDlg, IDC_BTN_STOP);

    // 设置控件的默认状态
    EnableWindow(this->btnSave, FALSE);
    EnableWindow(this->btnCopy, FALSE);
    EnableWindow(this->btnClear, FALSE);
    EnableWindow(this->btnStop, FALSE);

    ReadConfig();
    CheckDlgButton(hwndDlg, IDC_CHECK_MD5, this->md5);
    CheckDlgButton(hwndDlg, IDC_CHECK_CRC32, this->crc32);
    CheckDlgButton(hwndDlg, IDC_CHECK_CRC64, this->crc64);
    CheckDlgButton(hwndDlg, IDC_CHECK_SHA1, this->sha1);
    CheckDlgButton(hwndDlg, IDC_CHECK_UPPER, this->isUpper);
    
    // 设置文本框文字大小
    //PostMessage(this->editBox, EM_SETFONTSIZE, 2, 0);

    // 设置进度条的范围
    SendMessage(progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, this->proBarMaxRange));
    // 设置进度条步进长度
    SendMessage(progressBar, PBM_SETSTEP, 1, 0);

    //Edit_SetCueBannerText(this->editBox, L"浏览文件或拖拽文件到此处");
}

void MainDialog::EditAppendText(LPCTSTR text) const {
    if (text == nullptr) return;
    // 获取当前控件内的字符数
    /*const int nTextLen = GetWindowTextLength(this->editBox);
    // 给新字符串分配内存，并初始化
    const int bufferSize = nTextLen + lstrlen(text) + 1;
    const LPTSTR szResult = reinterpret_cast<LPTSTR>(new LPTSTR[bufferSize]);
    
    if (szResult == nullptr) return;
    memset(szResult, 0, bufferSize);

    // 以字符为单位复制文本框内容
    GetWindowText(this->editBox, szResult, nTextLen + 1);  // 第三个参数表示最大字符数，包括NULL
    lstrcat(&szResult[nTextLen], text);
    SetWindowText(this->editBox, szResult);
    
    delete[] szResult;*/

    SendMessage(this->editBox, EM_SETSEL, -2, -1); // 让光标移动到末尾
    SendMessage(this->editBox, EM_REPLACESEL, true, reinterpret_cast<LPARAM>(text)); // 替换光标处的内容（这里相当于append）
    SendMessage(this->editBox, WM_VSCROLL, SB_BOTTOM, 0); // 滚动到最后
}

void MainDialog::OnOpen(HWND hwndDlg, const UINT event) {
    // 只接受点击事件
    if (event != BN_CLICKED) return;

    SelectFileDialog(gFilePathVec, TRUE);

    if (gFilePathVec.empty()) return;

    gFreeType = TRUE;
    this->GetParams(hwndDlg);
    StartHashThread();
}

void MainDialog::OnSave(HWND hwndDlg, UINT event) const {
    COMDLG_FILTERSPEC fileType[]{
        {L"文本类型", L"*.txt"},
        {L"所有类型", L"*.*"},
    };
    const LPWSTR filePath = SaveFileDialog(L"校验结果.txt", fileType, ARRAYSIZE(fileType));
    if (!filePath) return;

    // 获取文本字符长度，+1 包括 null 终于符在内
    const int textLen = GetWindowTextLength(this->editBox) + 1;
    const LPTSTR resultStr = reinterpret_cast<LPTSTR>(new LPTSTR[textLen]);
    memset(resultStr, 0, textLen);
    
    GetWindowText(this->editBox, resultStr, textLen);
    // 这里要用二进制的方式原样输出，文本方式会多出一个 \r
    std::ofstream ofs(filePath, std::ios::out | std::ios::binary);

    if (ofs.is_open()) {
        char* cstr = nullptr;
        const size_t len = WcharToChar(&cstr, resultStr) - 1;

        ofs.write(cstr, static_cast<std::streamsize>(len));

        ofs.close();
        delete[] cstr;
    } else {
         MessageBox(hwndDlg, L"文件写入失败！", L"错误", MB_OK | MB_ICONERROR);
    }

    CoTaskMemFree(filePath);
    delete[] resultStr;
}

void MainDialog::OnCopy(HWND hwndDlg, UINT event) const {
    int bufferSize = GetWindowTextLength(this->editBox) + 1;
    const LPTSTR szStr = reinterpret_cast<LPTSTR>(new LPTSTR[bufferSize]);

    GetWindowText(this->editBox, szStr, bufferSize);  // 第三个参数表示最大字符数，包括 NULL
    bufferSize = static_cast<int>(sizeof(TCHAR) * bufferSize);
    // 申请一块全局内存
    // set 数据到剪贴版，数据需要放到全局内存中
    const HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, bufferSize);

    if (OpenClipboard(hwndDlg) && hData) {
        EmptyClipboard();
        const LPWSTR pData = static_cast<LPWSTR>(GlobalLock(hData)); // 锁定内存并返回指向该内存的指针
        CopyMemory(pData, szStr, bufferSize);
        GlobalUnlock(hData); // 解锁内存
        
        SetClipboardData(CF_UNICODETEXT, hData);
        CloseClipboard();

        // SetClipboardData 之后，内存所有权已移交给系统，不能再手动释放了
        //GlobalFree(pData);
    } else {
        std::wstring wstr = L"复制到剪贴板失败，错误码：" + std::to_wstring(GetLastError());
        MessageBox(hwndDlg, wstr.c_str(), L"意外的错误", MB_OK | MB_ICONERROR);
    }

    delete[] szStr;
}

void MainDialog::OnClear(HWND hwndDlg, UINT event) const {
    SetWindowText(this->editBox, nullptr);
    EnableWindow(this->btnSave, FALSE);
    EnableWindow(this->btnCopy, FALSE);
    EnableWindow(this->btnClear, FALSE);
}

void MainDialog::OnStop(HWND hwndDlg, UINT event) {
    this->isRun = FALSE;
}

void MainDialog::OnDropFiles(HWND hwndDlg, WPARAM wParam, LPARAM lParam) {
    if (gThatApp->isRun) {
        MessageBox(hwndDlg, L"请等待当前任务结束再操作！", L"任务正在执行...", MB_ICONASTERISK | MB_OK);
        return;
    }

    constexpr short bufferSize = sizeof(TCHAR) * MAX_PATH + 1;
    const HDROP hDrop = reinterpret_cast<HDROP>(wParam); // 拖拽文件句柄
    DWORD dwAttribute;
    TCHAR* szFilePath;
    const UINT count = DragQueryFile(hDrop, /*0xFFFFFFFF*/ static_cast<UINT>(-1), nullptr, 0);

    // 如果容器的容量小于文件数量，则扩展容量
    if (gFilePathVec.capacity() < count)
        gFilePathVec.reserve(count);

    for (UINT i = 0; i < count; i++) {
        szFilePath = new TCHAR[MAX_PATH + 1]; // 文件的路径
        memset(szFilePath, 0, bufferSize);

        DragQueryFile(hDrop, i, szFilePath, MAX_PATH); // 获取文件路径
        dwAttribute = GetFileAttributes(szFilePath);
        
        // 如果是文件夹就跳过
        if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) {
            delete[] szFilePath;
            continue;
        }

        gFilePathVec.push_back(szFilePath);
        // szFilePath 会在 FilePathVecFree 函数中 delete[]
    }

    DragFinish(hDrop);

    if (gFilePathVec.empty()) return;

    gFreeType = FALSE;
    this->GetParams(hwndDlg);
    StartHashThread();
}

void MainDialog::OnMenuAbout(HWND hwndDlg) const {
    // “关于”窗口的消息处理函数
    auto proc = [](HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        if (uMsg == WM_COMMAND) {
            const UINT cId = LOWORD(wParam);  // 控件ID
            if (cId == IDOK || cId == IDCANCEL) {
                EndDialog(hwndDlg, IDCANCEL);
                return TRUE;
            }
        }
        if (uMsg == WM_NOTIFY) {
            // 点击 SysLink Control 时
            if (reinterpret_cast<LPNMHDR>(lParam)->code == NM_CLICK) {
                ShellExecute(nullptr, L"open", reinterpret_cast<PNMLINK>(lParam)->item.szUrl,
                    nullptr, nullptr, SW_SHOW);
                return TRUE;
            }
        }
        return FALSE;
    };

    DialogBox(this->hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hwndDlg, proc);
}

void MainDialog::GetParams(HWND hwndDlg) {
    this->md5 = static_cast<BOOL>(IsDlgButtonChecked(hwndDlg, IDC_CHECK_MD5));
    this->crc32 = static_cast<BOOL>(IsDlgButtonChecked(hwndDlg, IDC_CHECK_CRC32));
    this->crc64 = static_cast<BOOL>(IsDlgButtonChecked(hwndDlg, IDC_CHECK_CRC64));
    this->sha1 = static_cast<BOOL>(IsDlgButtonChecked(hwndDlg, IDC_CHECK_SHA1));
    this->isUpper = static_cast<BOOL>(IsDlgButtonChecked(hwndDlg, IDC_CHECK_UPPER));
}
