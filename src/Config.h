#pragma once
#include "PCH.h"
#include <atomic>
#include <mutex>
#include <string>

class Config
{
public:
    static Config& Get() { static Config i; return i; }
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    void Load();

    int  GetIntervalMinutes() const noexcept { return m_interval.load(); }
    int  GetHotkeyCode()      const noexcept { return m_hotkey.load(); }
    bool GetAutoDisplay()     const noexcept { return m_autoDisplay.load(); }
    bool GetDebug()           const noexcept { return m_debug.load(); }

private:
    Config() = default;
    std::atomic<int>  m_interval{    5 };
    std::atomic<int>  m_hotkey{     49 };  // 49 = N
    std::atomic<bool> m_autoDisplay{ true };
    std::atomic<bool> m_debug{       false };
};
