// ���Ի����ʵ��

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

// FilePathVecFree �����������
// true ʹ�� CoTaskMemFree
// false ʹ�� delete[]
BOOL gFreeType = FALSE;

MainDialog* gThatApp = nullptr;
// ����ļ���·��
std::vector<LPWSTR> gFilePathVec;

// �ͷ� gFilePathVec �����е���Դ
void FilePathVecFree() {
    for (const auto value : gFilePathVec) {
        if (gFreeType) CoTaskMemFree(value);
        else delete[] value;
    }
    gFilePathVec.clear();
}

// �� hash ���������ı��༭��
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

// �����ļ��� Hash ֵ
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

    // ��ȡ�����ļ����ܴ�С
    for (const LPWSTR lpszFileName : gFilePathVec) {
        fileSize += GetFileSize(lpszFileName);
    }

    SendMessage(gThatApp->progressBar, PBM_SETPOS, 0, 0); // ���ý�����
    const size_t limit = fileSize / gThatApp->proBarMaxRange;

    // �����ļ����� hash ֵ
    for (const LPWSTR lpszFileName : gFilePathVec) {
        // ��ʾ�ļ����ƺʹ�С
        gThatApp->EditAppendText(L"�ļ�:\t");
        gThatApp->EditAppendText(lpszFileName);
        gThatApp->EditAppendText(L"\r\n");
        fileSize = GetFileSize(lpszFileName);

        wchar_t* fileSizeStrW = new wchar_t[20];
        wsprintf(fileSizeStrW, L"%d", fileSize);
        gThatApp->EditAppendText(L"��С:\t");
        gThatApp->EditAppendText(fileSizeStrW);
        gThatApp->EditAppendText(L" �ֽ�\r\n");
        delete[] fileSizeStrW;

        // ���δ��ѡ�κμ����������
        if (!gThatApp->md5 && !gThatApp->crc32 && !gThatApp->crc64 && !gThatApp->sha1) {
            gThatApp->EditAppendText(L"\r\n");
            continue;
        }

        // �ڼ���ǰ���� reset.��sha1.hpp �� final() ʱ���Զ� reset��
        md5->reset();
        crc32->Reset();
        crc64->Reset();

        ifstream ifs(lpszFileName, std::ios::in | std::ios::binary);
        if (!ifs.is_open()) continue;

        std::streamsize len;

        // ���Ĳ��֣��ļ��� hash ֵ�ڴ˵��ø��㷨����
        while (!ifs.eof()) {
            ifs.read(buffer, buffSize);
            len = ifs.gcount();

            if (gThatApp->md5) md5->update(reinterpret_cast<const byte*>(buffer), len);
            if (gThatApp->crc32) crc32->Update(reinterpret_cast<crc::Byte*>(buffer), len);
            if (gThatApp->crc64) crc64->Update(reinterpret_cast<crc::Byte*>(buffer), len);
            if (gThatApp->sha1) sha1->update(buffer, len);

            count += len;
            if (count >= limit) {
                // ����������
                SendMessage(gThatApp->progressBar, PBM_STEPIT, 0, 0);
                count = 0;
            }
            if (!gThatApp->isRun) {
                // �����߳�
                //_endthread();

                ifs.close();
                // ��˵������ʹ�� goto, �����������ǱȽϼ��ķ�ʽ��
                // ������� goto, Ҳ���������� break ����������ѭ��
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
        gThatApp->EditAppendText(L"������ֹ����\r\n\r\n");

    delete[] buffer;
    delete md5;
    delete crc32;
    delete crc64;
    delete sha1;
}

// ����һ�����̣߳��������߳��м����ļ� Hash ֵ
void StartHashThread() {
    auto callbackFun = [](void*) {
        gThatApp->isRun = TRUE;
        // ��ʼ����ʱ���ð�ť��Ӧ��״̬
        EnableWindow(gThatApp->btnOpen, FALSE);
        EnableWindow(gThatApp->btnStop, TRUE);
        // ִ��
        FilesHash();
        // ������ɺ����ð�ť��Ӧ��״̬
        EnableWindow(gThatApp->btnOpen, TRUE);
        EnableWindow(gThatApp->btnSave, TRUE);
        EnableWindow(gThatApp->btnCopy, TRUE);
        EnableWindow(gThatApp->btnClear, TRUE);
        EnableWindow(gThatApp->btnStop, FALSE);
        // ����ļ�·��
        FilePathVecFree();
        gThatApp->isRun = FALSE;
    };
    // �������߳�
    // �����߳��м����ļ� Hash ֵ�������������߳���ɽ��桰������
    _beginthread(callbackFun, 0, nullptr);
}

// ��ȡ����
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

// ��������
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
    // CreateDialog ���������һ������������Ϣ���������ֳƴ��ڹ��̣���������ĳ�Ա������
    // ����ͨ���������������ӵ��� MainDialog �ĳ�Ա���� DialogProc
    auto proc = [](HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        return gThatApp->DialogProc(hwndDlg, uMsg, wParam, lParam);
    };

    // ����һ����ģ̬�Ի���
    this->hDialog = CreateDialog(this->hInstance, MAKEINTRESOURCE(this->templateID), NULL, proc);
    // ��ʾ�Ի���
    if (hDialog) ShowWindow(this->hDialog, SW_SHOW);

    MSG msg;

    // ��Ϣѭ��
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!IsDialogMessage(this->hDialog, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

}

// ���ڹ��̣���Ϣ����
// �û������в�������ͨ��ϵͳ��Ϣ���ͣ�����������в�����Ϣϵͳ��������Ӧ����
INT_PTR MainDialog::DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_INITDIALOG:
          OnInitDialog(hwndDlg, wParam, lParam);
          break;
        case WM_COMMAND: {
            const UINT controlId = LOWORD(wParam);  // �ؼ�ID
            const UINT event = HIWORD(wParam);      // �¼�����
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

// �Ի��򴴽���ɺ󣬽��г�ʼ��
void MainDialog::OnInitDialog(HWND hwndDlg, WPARAM wParam, LPARAM lParam) {
     // ���ô���ͼ��
    SendMessage(hwndDlg, WM_SETICON, ICON_SMALL,
        (LPARAM) LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON_SMALL)) );
    SendMessage(hwndDlg, WM_SETICON, ICON_BIG,
        (LPARAM) LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_ICON_SMALL)) );
    
    // ���ϵͳ�˵���
    const HMENU hMenu = GetSystemMenu(hwndDlg, FALSE);
    AppendMenu(hMenu, MF_STRING , IDM_ABOUT, L"����(&A)");

    // ��ȡ�ؼ��ľ��
    this->progressBar = GetDlgItem(hwndDlg, IDC_PROGRESS_1);
    this->editBox = GetDlgItem(hwndDlg, IDC_RICHEDIT_RESULT);
    this->btnOpen = GetDlgItem(hwndDlg, IDC_BTN_OPEN);
    this->btnSave = GetDlgItem(hwndDlg, IDC_BTN_SAVE);
    this->btnCopy = GetDlgItem(hwndDlg, IDC_BTN_COPY);
    this->btnClear = GetDlgItem(hwndDlg, IDC_BTN_CLEAR);
    this->btnStop = GetDlgItem(hwndDlg, IDC_BTN_STOP);

    // ���ÿؼ���Ĭ��״̬
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
    
    // �����ı������ִ�С
    //PostMessage(this->editBox, EM_SETFONTSIZE, 2, 0);

    // ���ý������ķ�Χ
    SendMessage(progressBar, PBM_SETRANGE, 0, MAKELPARAM(0, this->proBarMaxRange));
    // ���ý�������������
    SendMessage(progressBar, PBM_SETSTEP, 1, 0);

    //Edit_SetCueBannerText(this->editBox, L"����ļ�����ק�ļ����˴�");
}

void MainDialog::EditAppendText(LPCTSTR text) const {
    if (text == nullptr) return;
    // ��ȡ��ǰ�ؼ��ڵ��ַ���
    /*const int nTextLen = GetWindowTextLength(this->editBox);
    // �����ַ��������ڴ棬����ʼ��
    const int bufferSize = nTextLen + lstrlen(text) + 1;
    const LPTSTR szResult = reinterpret_cast<LPTSTR>(new LPTSTR[bufferSize]);
    
    if (szResult == nullptr) return;
    memset(szResult, 0, bufferSize);

    // ���ַ�Ϊ��λ�����ı�������
    GetWindowText(this->editBox, szResult, nTextLen + 1);  // ������������ʾ����ַ���������NULL
    lstrcat(&szResult[nTextLen], text);
    SetWindowText(this->editBox, szResult);
    
    delete[] szResult;*/

    SendMessage(this->editBox, EM_SETSEL, -2, -1); // �ù���ƶ���ĩβ
    SendMessage(this->editBox, EM_REPLACESEL, true, reinterpret_cast<LPARAM>(text)); // �滻��괦�����ݣ������൱��append��
    SendMessage(this->editBox, WM_VSCROLL, SB_BOTTOM, 0); // ���������
}

void MainDialog::OnOpen(HWND hwndDlg, const UINT event) {
    // ֻ���ܵ���¼�
    if (event != BN_CLICKED) return;

    SelectFileDialog(gFilePathVec, TRUE);

    if (gFilePathVec.empty()) return;

    gFreeType = TRUE;
    this->GetParams(hwndDlg);
    StartHashThread();
}

void MainDialog::OnSave(HWND hwndDlg, UINT event) const {
    COMDLG_FILTERSPEC fileType[]{
        {L"�ı�����", L"*.txt"},
        {L"��������", L"*.*"},
    };
    const LPWSTR filePath = SaveFileDialog(L"У����.txt", fileType, ARRAYSIZE(fileType));
    if (!filePath) return;

    // ��ȡ�ı��ַ����ȣ�+1 ���� null ���ڷ�����
    const int textLen = GetWindowTextLength(this->editBox) + 1;
    const LPTSTR resultStr = reinterpret_cast<LPTSTR>(new LPTSTR[textLen]);
    memset(resultStr, 0, textLen);
    
    GetWindowText(this->editBox, resultStr, textLen);
    // ����Ҫ�ö����Ƶķ�ʽԭ��������ı���ʽ����һ�� \r
    std::ofstream ofs(filePath, std::ios::out | std::ios::binary);

    if (ofs.is_open()) {
        char* cstr = nullptr;
        const size_t len = WcharToChar(&cstr, resultStr) - 1;

        ofs.write(cstr, static_cast<std::streamsize>(len));

        ofs.close();
        delete[] cstr;
    } else {
         MessageBox(hwndDlg, L"�ļ�д��ʧ�ܣ�", L"����", MB_OK | MB_ICONERROR);
    }

    CoTaskMemFree(filePath);
    delete[] resultStr;
}

void MainDialog::OnCopy(HWND hwndDlg, UINT event) const {
    int bufferSize = GetWindowTextLength(this->editBox) + 1;
    const LPTSTR szStr = reinterpret_cast<LPTSTR>(new LPTSTR[bufferSize]);

    GetWindowText(this->editBox, szStr, bufferSize);  // ������������ʾ����ַ��������� NULL
    bufferSize = static_cast<int>(sizeof(TCHAR) * bufferSize);
    // ����һ��ȫ���ڴ�
    // set ���ݵ������棬������Ҫ�ŵ�ȫ���ڴ���
    const HANDLE hData = GlobalAlloc(GMEM_MOVEABLE, bufferSize);

    if (OpenClipboard(hwndDlg) && hData) {
        EmptyClipboard();
        const LPWSTR pData = static_cast<LPWSTR>(GlobalLock(hData)); // �����ڴ沢����ָ����ڴ��ָ��
        CopyMemory(pData, szStr, bufferSize);
        GlobalUnlock(hData); // �����ڴ�
        
        SetClipboardData(CF_UNICODETEXT, hData);
        CloseClipboard();

        // SetClipboardData ֮���ڴ�����Ȩ���ƽ���ϵͳ���������ֶ��ͷ���
        //GlobalFree(pData);
    } else {
        std::wstring wstr = L"���Ƶ�������ʧ�ܣ������룺" + std::to_wstring(GetLastError());
        MessageBox(hwndDlg, wstr.c_str(), L"����Ĵ���", MB_OK | MB_ICONERROR);
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
        MessageBox(hwndDlg, L"��ȴ���ǰ��������ٲ�����", L"��������ִ��...", MB_ICONASTERISK | MB_OK);
        return;
    }

    constexpr short bufferSize = sizeof(TCHAR) * MAX_PATH + 1;
    const HDROP hDrop = reinterpret_cast<HDROP>(wParam); // ��ק�ļ����
    DWORD dwAttribute;
    TCHAR* szFilePath;
    const UINT count = DragQueryFile(hDrop, /*0xFFFFFFFF*/ static_cast<UINT>(-1), nullptr, 0);

    // �������������С���ļ�����������չ����
    if (gFilePathVec.capacity() < count)
        gFilePathVec.reserve(count);

    for (UINT i = 0; i < count; i++) {
        szFilePath = new TCHAR[MAX_PATH + 1]; // �ļ���·��
        memset(szFilePath, 0, bufferSize);

        DragQueryFile(hDrop, i, szFilePath, MAX_PATH); // ��ȡ�ļ�·��
        dwAttribute = GetFileAttributes(szFilePath);
        
        // ������ļ��о�����
        if (dwAttribute & FILE_ATTRIBUTE_DIRECTORY) {
            delete[] szFilePath;
            continue;
        }

        gFilePathVec.push_back(szFilePath);
        // szFilePath ���� FilePathVecFree ������ delete[]
    }

    DragFinish(hDrop);

    if (gFilePathVec.empty()) return;

    gFreeType = FALSE;
    this->GetParams(hwndDlg);
    StartHashThread();
}

void MainDialog::OnMenuAbout(HWND hwndDlg) const {
    // �����ڡ����ڵ���Ϣ������
    auto proc = [](HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) -> INT_PTR {
        if (uMsg == WM_COMMAND) {
            const UINT cId = LOWORD(wParam);  // �ؼ�ID
            if (cId == IDOK || cId == IDCANCEL) {
                EndDialog(hwndDlg, IDCANCEL);
                return TRUE;
            }
        }
        if (uMsg == WM_NOTIFY) {
            // ��� SysLink Control ʱ
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
