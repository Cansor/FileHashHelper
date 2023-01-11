﻿#pragma once

// // 包含 SDKDDKVer.h 可定义可用的最高版本的 Windows 平台。
// 如果希望为之前的 Windows 平台构建应用程序，在包含 SDKDDKVer.h 之前请先包含 WinSDKVer.h 并
// 将 _WIN32_WINNT 宏设置为想要支持的平台。

#define WINVER       _WIN32_WINNT_WIN7
#define _WIN32_WINNT _WIN32_WINNT_WIN7

//#define NOCOMM  // 排除掉一些不用的API

#include <winsdkver.h>
#include <SDKDDKVer.h>
