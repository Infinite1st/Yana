#include "PCH.h"
#include "Config.h"
#include <SimpleIni.h>
#include <filesystem>

void Config::Load()
{
    auto dir = SKSE::log::log_directory();
    if (!dir) return;

    std::filesystem::path path = "Data\\SKSE\\Plugins\\PlayerCount.ini";

    CSimpleIniA ini;
    ini.SetUnicode();

    if (ini.LoadFile(path.string().c_str()) < 0) {
        ini.SetLongValue("General", "IntervalMinutes", 5,
            "; Интервал между уведомлениями в минутах (1-60)");
        ini.SetLongValue("General", "HotkeyCode", 49,
            "; Scancode кнопки для ручного запроса (49 = N)");
        ini.SetLongValue("General", "AutoDisplay", 1,
            "; Автоматические уведомления по таймеру (1 = вкл, 0 = выкл)");
        ini.SetLongValue("General", "Debug", 0,
            "; Показывать тег [Auto]/[Manual] перед сообщением (1 = вкл, 0 = выкл)");
        ini.SaveFile(path.string().c_str());
        return;
    }

    int v = static_cast<int>(ini.GetLongValue("General", "IntervalMinutes", 5));
    m_interval.store(std::clamp(v, 1, 60));

    int h = static_cast<int>(ini.GetLongValue("General", "HotkeyCode", 49));
    m_hotkey.store(h);

    int a = static_cast<int>(ini.GetLongValue("General", "AutoDisplay", 1));
    m_autoDisplay.store(a != 0);

    int d = static_cast<int>(ini.GetLongValue("General", "Debug", 0));
    m_debug.store(d != 0);
}
