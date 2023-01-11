// FileHashHelper.cpp : 定义应用程序的入口点。

#include "Resource.h"
#include "MainDialog.h"


// 启用视觉样式
// 添加以下编译器指令或在资源文件中添加 AppName.exe.manifest 清单文件
// 详情参考 https://learn.microsoft.com/en-us/windows/win32/controls/cookbook-overview
#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' \
    version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' \
    version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif 


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // 加载必要的库
    LoadLibrary(TEXT("Riched32.dll"));
    // 创建主对话框
    MainDialog(IDD_DIALOG_MIAN, hInstance).Show();

    return TRUE;
}
