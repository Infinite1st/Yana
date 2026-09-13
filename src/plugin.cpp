#include "PCH.h"
#include "Config.h"
#include "SteamPlayerService.h"

namespace
{
    // RE::DebugNotification was removed from CommonLibSSE-NG; restored locally
    // using the same relocation IDs the library used to expose it with.
    void DebugNotification(const char* a_notification, const char* a_soundToPlay = nullptr, bool a_cancelIfAlreadyQueued = false)
    {
        using func_t = decltype(&DebugNotification);
        static REL::Relocation<func_t> func{ RELOCATION_ID(52050, 52933) };
        return func(a_notification, a_soundToPlay, a_cancelIfAlreadyQueued);
    }

    void OnFetch(int count, bool connected, bool manual)
    {
        std::string msg;
        if (Config::Get().GetDebug()) {
            msg = (manual ? "[Manual] " : "[Auto] ");
        }
        msg += connected
            ? "Players in-game: " + std::to_string(count)
            : "Players in-game: N/A";

        if (auto* task = SKSE::GetTaskInterface()) {
            task->AddTask([msg]() { DebugNotification(msg.c_str()); });
        }
    }

    class InputSink : public RE::BSTEventSink<RE::InputEvent*>
    {
    public:
        static InputSink* Get() { static InputSink i; return &i; }

        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event,
                                              RE::BSTEventSource<RE::InputEvent*>*) override
        {
            if (!a_event) return RE::BSEventNotifyControl::kContinue;

            int hotkey = Config::Get().GetHotkeyCode();
            if (hotkey == 0) return RE::BSEventNotifyControl::kContinue;

            for (auto* ev = *a_event; ev; ev = ev->next) {
                auto* btn = ev->AsButtonEvent();
                if (!btn || !btn->IsDown()) continue;

                if (btn->GetIDCode() == static_cast<uint32_t>(hotkey)) {
                    SteamPlayerService::Get().FetchNow();
                }
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };

    void MessageHandler(SKSE::MessagingInterface::Message* a_msg)
    {
        if (a_msg->type == SKSE::MessagingInterface::kDataLoaded) {
            SteamPlayerService::Get().SetCallback(OnFetch);
            SteamPlayerService::Get().Start();

            RE::BSInputDeviceManager::GetSingleton()->AddEventSink(InputSink::Get());
        }
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{
    SKSE::Init(skse);

    Config::Get().Load();

    auto* messaging = SKSE::GetMessagingInterface();
    if (!messaging->RegisterListener("SKSE", MessageHandler)) {
        return false;
    }

    return true;
}
