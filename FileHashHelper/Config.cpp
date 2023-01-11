#include "Config.h"

#include <string>


ConfigReg::ConfigReg() {
    this->openKey = nullptr;
    this->hKey = nullptr;
    this->subKay = nullptr;
}

LSTATUS ConfigReg::CreateKey(const HKEY hKye, const LPCTSTR lpSubKey, const REGSAM samDesired) {
    //unsigned long l = REG_CREATED_NEW_KEY;
    this->subKay = lpSubKey;
    this->hKey = hKye;
    return RegCreateKeyEx(hKye, lpSubKey, 0, nullptr, REG_OPTION_NON_VOLATILE, samDesired,
        nullptr, &this->openKey, nullptr);
}

LSTATUS ConfigReg::SetValue(const wchar_t* valueName, const DWORD dwType, const BYTE* data, const DWORD len) const {
    return RegSetValueEx(this->openKey, valueName, 0, dwType, data, len);
}

LSTATUS ConfigReg::GetValue(const wchar_t* valueName, const PVOID pvData, const LPDWORD dataLen) const {
    return RegGetValue(this->openKey, nullptr, valueName, RRF_RT_ANY, nullptr, pvData, dataLen);
}

LSTATUS ConfigReg::DeleteKey(const REGSAM samDesired) const {
    return RegDeleteKeyEx(this->hKey, this->subKay, samDesired, 0);
}

LSTATUS ConfigReg::DeleteValue(const wchar_t* valueName) const {
    return RegDeleteValue(this->openKey, valueName);
}

LSTATUS ConfigReg::Close() const {
    return RegCloseKey(this->openKey);
}
