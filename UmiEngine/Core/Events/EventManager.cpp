module;
#include <string>
#include <memory>
module EventManager;

using namespace Umi;

void EventManager::PushOpenLevelEvent(const std::string& levelName)
{
	eventQueue.push_back(std::make_unique<OpenLevel>(levelName));
}

void EventManager::PushCloseGameEvent()
{
	eventQueue.push_back(std::make_unique<CloseGame>());
}

void EventManager::ProcessEvents()
{
	for (auto& event : eventQueue)
	{
		switch (event->type)
		{
		case EVENT_TYPE::OPEN_LEVEL:
		{
			if (auto eventBuffer = dynamic_cast<OpenLevel*>(event.get()))
				onOpenLevel(eventBuffer->levelName);

			break;
		}

		case EVENT_TYPE::CLOSE_GAME:
		{
			onCloseGame();
			break;
		}

		default:
			break;
		}
	}

	eventQueue.clear();
}
