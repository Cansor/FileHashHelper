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

        // �����¼���ڼ���֮ǰ�������һ��
        void Reset();
        // �ִμ��� CRC32�����Զ�ε��� Update�������ڼ�������ļ�ʱ
        void Update(Byte* bytes, std::streamsize length);
        // ��ɼ��㲢���ؼ���Ľ����Ϊʮ�����Ƶ��ַ���
        // upper Ϊ true ʱΪ��д
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

        // �����¼���ڼ���֮ǰ�������һ��
        void Reset();
        // �ִμ��� CRC64�����Զ�ε��� Update�������ڼ�������ļ�ʱ
        void Update(Byte* bytes, std::streamsize length);
        // ��ɼ��㲢���ؼ���Ľ����Ϊʮ�����Ƶ��ַ���
        // upper Ϊ true ʱΪ��д
        std::string Finish(bool upper = false);
        std::wstring FinishW(bool upper = false);

      private:
        uint64_t* crcTable_64;
        uint64_t crc;
    };

}

