#include "FileUtils.h"

void SelectFileDialog(std::vector<LPWSTR> &vec, const BOOL multiSelect, COMDLG_FILTERSPEC* fileType,
    const UINT fileTypeLen, const UINT fileTypeIndex) {
    IFileOpenDialog* fileOpenDialog = nullptr;
    HRESULT result = CoCreateInstance(CLSID_FileOpenDialog, nullptr,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileOpenDialog));
    if (FAILED(result)) return;

    DWORD fos;
    result = fileOpenDialog->GetOptions(&fos);
    result = fileOpenDialog->SetOptions(fos | FOS_FORCEFILESYSTEM | (multiSelect ? FOS_ALLOWMULTISELECT : 0));

    // �ļ�������
    if (!fileType) {
        COMDLG_FILTERSPEC t[] = {
          {L"�����ļ�", L"*.*"},
      };
        fileType = t;
    }

    result = fileOpenDialog->SetFileTypes(fileTypeLen, fileType); // �����ļ�����������Ŀ���ļ�����������
    result = fileOpenDialog->SetFileTypeIndex(fileTypeIndex); // Ĭ��ѡ�е��ļ���������
    result = fileOpenDialog->Show(nullptr);
    if (FAILED(result)) return;

    IShellItemArray* selItemArr;
    result = fileOpenDialog->GetResults(&selItemArr);

    if (SUCCEEDED(result)) {
        LPWSTR filePath = nullptr; // �ļ�ȫ·��
        DWORD itemNum = 0; // ѡ����ļ�����
        result = selItemArr->GetCount(&itemNum);
        // �������������С���ļ�����������չ����
        if (vec.capacity() < itemNum) vec.reserve(itemNum);

        for (DWORD i = 0; i < itemNum; i++) {
			IShellItem* fileItem = nullptr;
			result = selItemArr->GetItemAt(i, &fileItem);
			if (FAILED(result)) continue;

            result = fileItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
            if (SUCCEEDED(result)) {
                vec.push_back(filePath);
                // CoTaskMemFree(filePath);
            }

            fileItem->Release();
		}

        selItemArr->Release();
    }

    fileOpenDialog->Release();
}

LPWSTR SaveFileDialog(const LPCTSTR fileName, COMDLG_FILTERSPEC* fileType, const UINT fileTypeLen, const UINT fileTypeIndex) {
    IFileSaveDialog* fileSaveDialog = nullptr;
    HRESULT result = CoCreateInstance(CLSID_FileSaveDialog, nullptr,
        CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileSaveDialog));
    if (FAILED(result)) return nullptr;

    DWORD fos;
    result = fileSaveDialog->GetOptions(&fos);
    result = fileSaveDialog->SetOptions(fos | FOS_FORCEFILESYSTEM);
    fileSaveDialog->SetFileName(fileName);

    if (!fileType) {
        COMDLG_FILTERSPEC t[] = {
            {L"�����ļ�", L"*.*"},
        };
        fileType = t;
    }

    result = fileSaveDialog->SetFileTypes(fileTypeLen, fileType);  // �����ļ�����������Ŀ���ļ�����������
    result = fileSaveDialog->SetFileTypeIndex(fileTypeIndex);  // Ĭ��ѡ�е��ļ���������
    result = fileSaveDialog->Show(nullptr);
    if (FAILED(result)) return nullptr;

    LPWSTR filePath = nullptr; // �ļ�ȫ·��
    IShellItem* shellItem;
    result = fileSaveDialog->GetResult(&shellItem);

    if (SUCCEEDED(result)) {
        result = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
        if (FAILED(result)) filePath = nullptr;
    }

    fileSaveDialog->Release();
    return filePath;
}

int64_t GetFileSize(const LPWSTR fileName) {
    const HANDLE file = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ,
                       (LPSECURITY_ATTRIBUTES)nullptr, OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return -1;

    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(file, &fileSize)) return -1;

    /*DWORD sizeHeight;
    const DWORD sizeLow = GetFileSize(hFile, &sizeHeight);
    uint64_t fileSize = sizeHeight;
    fileSize <<= 32;
    fileSize |= sizeLow;*/

    CloseHandle(file);
    return fileSize.QuadPart;
}