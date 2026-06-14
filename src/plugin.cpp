#include "PCH.h"
#include "Config.h"
#include "SteamPlayerService.h"

namespace
{
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
            task->AddTask([msg]() { RE::DebugNotification(msg.c_str()); });
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

            for (auto* ev = *a_event; ev; ev = ev->next) {
                auto* btn = ev->AsButtonEvent();
                if (!btn || !btn->IsDown()) continue;

                if (btn->GetIDCode() == static_cast<uint32_t>(Config::Get().GetHotkeyCode())) {
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

            if (Config::Get().GetAutoDisplay()) {
                SteamPlayerService::Get().Start();
            }

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
