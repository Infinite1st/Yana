#pragma once
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

class SteamPlayerService
{
public:
    static SteamPlayerService& Get() { static SteamPlayerService i; return i; }
    SteamPlayerService(const SteamPlayerService&) = delete;
    SteamPlayerService& operator=(const SteamPlayerService&) = delete;

    void Start();
    void Stop();
    void FetchNow();  // ручной вызов (кнопка)

    using Callback = std::function<void(int count, bool connected, bool manual)>;
    void SetCallback(Callback cb) { std::lock_guard l(m_cbMx); m_cb = std::move(cb); }

private:
    SteamPlayerService() = default;
    void Loop();
    void Fetch(bool manual);

    static constexpr const char* kUrl =
        "https://api.steampowered.com/ISteamUserStats/GetNumberOfCurrentPlayers/v1/?appid=489830";

    std::thread             m_thread;
    std::thread             m_manualThread;
    std::atomic<bool>       m_running{ false };
    std::mutex              m_wakeMx;
    std::condition_variable m_cv;
    std::mutex              m_cbMx;
    Callback                m_cb;
};
