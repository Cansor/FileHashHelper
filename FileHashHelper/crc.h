#pragma once
#include <string>


namespace crc {
    constexpr uint32_t CRC32_POLY = 0xEDB88320;
    constexpr uint32_t CRC32_INITIAL = 0xFFFFFFFF;
    constexpr uint64_t CRC64_XZ_POLY = 0xC96C5795D7870F42ULL;
    constexpr uint64_t CRC64_INITIAL = 0xFFFFFFFFFFFFFFFFULL;

    typedef unsigned char Byte;


    class CRC32 {
      public:
        CRC32();
        ~CRC32();

        // 清除记录。在计算之前必需调用一次
        void Reset();
        // 分次计算 CRC32，可以多次调用 Update，比如在计算大型文件时
        void Update(Byte* bytes, std::streamsize length);
        // 完成计算并返回计算的结果，为十六进制的字符串
        // upper 为 true 时为大写
        std::string Finish(bool upper = false);
        std::wstring FinishW(bool upper = false);

      private:
        uint32_t* crcTable_32;
        uint32_t crc;

    };


    class  XzCRC64 {
      public:
        XzCRC64();
        ~XzCRC64();

        // 清除记录。在计算之前必需调用一次
        void Reset();
        // 分次计算 CRC64，可以多次调用 Update，比如在计算大型文件时
        void Update(Byte* bytes, std::streamsize length);
        // 完成计算并返回计算的结果，为十六进制的字符串
        // upper 为 true 时为大写
        std::string Finish(bool upper = false);
        std::wstring FinishW(bool upper = false);

      private:
        uint64_t* crcTable_64;
        uint64_t crc;
    };

}

