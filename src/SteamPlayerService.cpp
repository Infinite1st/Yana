#include "PCH.h"
#include "SteamPlayerService.h"
#include "Config.h"
#include "HttpClient.h"
#include <nlohmann/json.hpp>

void SteamPlayerService::Start()
{
    bool exp = false;
    if (!m_running.compare_exchange_strong(exp, true)) return;
    m_thread = std::thread(&SteamPlayerService::Loop, this);
}

void SteamPlayerService::Stop()
{
    if (!m_running.exchange(false)) return;
    m_cv.notify_all();
    if (m_thread.joinable()) m_thread.join();
    if (m_manualThread.joinable()) m_manualThread.join();
}

void SteamPlayerService::Loop()
{
    int minutes = Config::Get().GetIntervalMinutes();
    if (minutes == 0) {
        m_running.store(false);
        return;
    }

    while (m_running.load()) {
        Fetch(false);

        std::unique_lock lk(m_wakeMx);
        m_cv.wait_for(lk, std::chrono::minutes(minutes),
                      [this] { return !m_running.load(); });
    }
}

void SteamPlayerService::Fetch(bool manual)
{
    HttpClient client;
    auto res = client.Get(kUrl, 10);

    if (!res.ok) {
        Callback cb;
        { std::lock_guard l(m_cbMx); cb = m_cb; }
        if (cb) cb(-1, false, manual);
        return;
    }

    try {
        auto j     = nlohmann::json::parse(res.body);
        int  count = j.at("response").at("player_count").get<int>();
        Callback cb;
        { std::lock_guard l(m_cbMx); cb = m_cb; }
        if (cb) cb(count, true, manual);
    } catch (const std::exception&) {
        Callback cb;
        { std::lock_guard l(m_cbMx); cb = m_cb; }
        if (cb) cb(-1, false, manual);
    }
}

void SteamPlayerService::FetchNow()
{
    if (m_manualThread.joinable()) m_manualThread.join();
    m_manualThread = std::thread(&SteamPlayerService::Fetch, this, true);
}
