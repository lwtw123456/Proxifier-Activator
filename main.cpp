#include <windows.h>

#include <string>
#include <string_view>

#include "keygen.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    constexpr auto path = L"Software\\Initex\\Proxifier\\License";
    const std::string owner = "Ada Lovelace";
    const std::string key = keygen::generate_key();

    HKEY hKey = nullptr;

    LSTATUS result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        path,
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_SET_VALUE,
        nullptr,
        &hKey,
        nullptr
    );

    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueExA(
            hKey,
            "Owner",
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(owner.c_str()),
			static_cast<DWORD>(owner.size() + 1)
        );
    }

    if (result == ERROR_SUCCESS)
    {
        result = RegSetValueExA(
            hKey,
            "Key",
            0,
            REG_SZ,
            reinterpret_cast<const BYTE*>(key.c_str()),
            static_cast<DWORD>(key.size() + 1)
        );
    }

    if (hKey)
        RegCloseKey(hKey);

    if (result != ERROR_SUCCESS)
    {
        wchar_t msg[128];
        swprintf_s(msg, L"注册失败，错误码: %ld", result);

        MessageBoxW(
            nullptr,
            msg,
            L"错误",
            MB_OK | MB_ICONERROR
        );

        return 1;
    }

    MessageBoxW(
        nullptr,
        L"注册成功，重启 Proxifier 即可。",
        L"成功",
        MB_OK | MB_ICONINFORMATION
    );

    return 0;
}