// 文件操作相关

#pragma once
#include <Shlobj.h>
#include <vector>

/// <summary>
/// 打开文件选择对话框。
/// 
/// 注意，vector 容器中的资源不能直接清除，在此之前需先
/// 调用 CoTaskMemFree 函数释放内存。
/// </summary>
/// <param name="vec">用于保存文件路径的容器</param>
/// <param name="multiSelect">true 时允许多选</param>
/// <param name="fileType">文件类型过滤器数组</param>
/// <param name="fileTypeLen">数组的长度</param>
/// <param name="fileTypeIndex">默认选择的过滤器的索引</param>
void SelectFileDialog(std::vector<LPWSTR> &vec, BOOL multiSelect = FALSE, COMDLG_FILTERSPEC* fileType = nullptr,
    UINT fileTypeLen = 1, UINT fileTypeIndex = 1);

/**
 * \brief 打开保存文件对话框。
 *        注意，返回的结果需调用 CoTaskMemFree 函数释放内存。
 * \param fileName 默认保存的文件名
 * \param fileType 文件类型过滤器数组
 * \param fileTypeLen 数组的长度
 * \param fileTypeIndex 默认选择的过滤器的索引
 * \return 选择的文件全路径
 */
LPWSTR SaveFileDialog(LPCTSTR fileName = nullptr, COMDLG_FILTERSPEC* fileType = nullptr,
    UINT fileTypeLen = 1, UINT fileTypeIndex = 1);

/// <summary> 获取文件大小</summary>
/// <param name="fileName">文件全路径名</param>
/// <returns>文件的 Byte 大小，如果失败则返回 -1</returns>
int64_t GetFileSize(LPWSTR fileName);