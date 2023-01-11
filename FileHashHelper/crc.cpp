#include "crc.h"
#include <sstream>

using namespace crc;

CRC32::CRC32(){
    this->Reset();
    uint32_t temp;
    this->crcTable_32 = new uint32_t[256];
    for (int i = 0; i < 256; i++) {
        temp = i;
        for (int j = 8; j > 0; j--) {
            if (temp & 1)
                temp = (temp >> 1) ^ CRC32_POLY;
            else
                temp >>= 1;
        }
        this->crcTable_32[i] = temp;
    }
}

CRC32::~CRC32() {
    delete[] this->crcTable_32;
}

void CRC32::Reset() {
    this->crc = CRC32_INITIAL;
}

void CRC32::Update(Byte* bytes, std::streamsize length) {
    while (length--) {
        this->crc = ((this->crc >> 8) & 0x00FFFFFF) ^ this->crcTable_32[(this->crc ^ *bytes) & 0xFF];
        bytes++;
    }
}

std::string CRC32::Finish(bool upper) {
     const auto buf = new char[9];
     sprintf_s(buf, 9, upper ? "%08X" : "%08x", this->crc ^ CRC32_INITIAL);
     std::string str(buf);
     delete[] buf;
    return str;
}

std::wstring CRC32::FinishW(bool upper) {
    std::wstringstream result;
    if (upper) result << std::uppercase;
    result << std::hex << (this->crc ^ CRC32_INITIAL);
    return result.str();
}


crc::XzCRC64::XzCRC64() {
    this->Reset();
    uint64_t temp;
    this->crcTable_64 = new uint64_t[256];
    for (int i = 0; i < 256; i++) {
        temp = i;
        for (int j = 0; j < 8; j++) {
            if (temp & 1)
                temp = (temp >> 1) ^ CRC64_XZ_POLY;
            else
                temp >>= 1;
        }
        this->crcTable_64[i] = temp;
    }
}

XzCRC64::~XzCRC64() {
    delete[] this->crcTable_64;
}

void XzCRC64::Reset() {
    this->crc = CRC64_INITIAL;
}


void XzCRC64::Update(Byte* bytes, std::streamsize length) {
    while (length--) {
        this->crc = this->crcTable_64[(this->crc ^ *bytes) & 0xff] ^ (this->crc >> 8);
        bytes++;
    }
}

std::string XzCRC64::Finish(bool upper) {
    const auto buf = new char[17];
    const uint64_t crcResult = this->crc ^ CRC64_INITIAL;
    const uint32_t low = crcResult & 0xffffffff,
                   high = (crcResult >> 32) & 0xffffffff;

    sprintf_s(buf, 17, upper ? "%08X%08X" : "%08x%08x", high, low);
    std::string str(buf);
    delete[] buf;
    return str;
}

std::wstring XzCRC64::FinishW(bool upper) {
    std::wstringstream result;
    if (upper) result << std::uppercase;
    result << std::hex << (this->crc ^ CRC64_INITIAL);
    return result.str();
}
