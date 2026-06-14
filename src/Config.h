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

    // Интервал между уведомлениями в минутах (1–60)
    int GetIntervalMinutes() const noexcept { return m_interval.load(); }

    // Scancode кнопки для ручного запроса
    int GetHotkeyCode() const noexcept { return m_hotkey.load(); }

private:
    Config() = default;
    std::atomic<int> m_interval{ 5 };
    std::atomic<int> m_hotkey{ 49 };  // 49 = N
};
