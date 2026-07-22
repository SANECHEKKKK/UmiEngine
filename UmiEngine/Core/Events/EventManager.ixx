module;
#include <EngineApi/EngineApi.h>
#include <vector>
#include <string>
#include <memory>
#include <functional>
export module EventManager;

export namespace Umi
{
    enum class EVENT_TYPE
    {
        OPEN_LEVEL,
        CLOSE_GAME,
        COUNT
    };

    struct Event
    {
        EVENT_TYPE type;

        explicit Event(EVENT_TYPE t) : type(t) {}
        virtual ~Event() = default;
    };

    struct OpenLevel : public Event
    {
        std::string levelName;

        explicit OpenLevel(std::string name)
            : Event(EVENT_TYPE::OPEN_LEVEL), levelName(std::move(name)) {
        }
    };

    struct CloseGame : public Event
    {
        explicit CloseGame()
            : Event(EVENT_TYPE::CLOSE_GAME) {
        }
    };

    class ENGINE_API EventManager
    {
    private:
        std::vector<std::unique_ptr<Event>> eventQueue;
        std::function<void(const std::string&)> onOpenLevel;
        std::function<void()> onCloseGame;

    public:
        EventManager() = default;

        EventManager(const EventManager&) = delete;
        EventManager& operator=(const EventManager&) = delete;

        void SetOpenLevelHandler(std::function<void(const std::string&)> h)
        {
            onOpenLevel = std::move(h);
        }
        void SetCloseGameHandler(std::function<void()> h)
        {
            onCloseGame = std::move(h);
        }

        void PushOpenLevelEvent(const std::string& levelName);
        void PushCloseGameEvent();
        void ProcessEvents();
    };
}