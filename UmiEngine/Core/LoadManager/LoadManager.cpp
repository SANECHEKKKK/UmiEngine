#include <LoadManager/LoadManager.h>
#include <EngineContext/EngineContext.h>
#include <Prefab/PrefabRequest.h>
#include <Prefab/PrefabManager.h>
#include <Texture/TextureManager.h>
#include <Model/ModelManager.h>

#include <filesystem>

using namespace Umi;

void LoadManager::LoadThread(PrefabRequest request)
{
	if (worker.joinable())
		worker.join();

	finished = false;


	worker = std::thread([this, request = std::move(request)]() {

		if (request.folders.empty())
		{
			finished = true;
			return;
		}

		std::vector<std::string> folderPaths;
		for (const auto& folder : request.folders)
		{
			folderPaths.push_back("Assets\\Prefabs\\" + folder);
		}

		for (const auto& folderPath : folderPaths)
		{
			if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
			{
				finished = true;
				return;
			}
		}

		std::vector<std::pair<std::string, std::string>> prefabsToLoad;
		try
		{
			for (const auto& folderPath : folderPaths)
			{
				for (const auto& entry : std::filesystem::directory_iterator(folderPath))
				{
					if (entry.is_regular_file() && entry.path().extension() == ".json")
					{
						std::string prefabName = entry.path().stem().string();

						if (std::find_if(prefabsToLoad.begin(), prefabsToLoad.end(), [&](const auto& pair) {
							return pair.first == prefabName;
							}) == prefabsToLoad.end())
						{
							prefabsToLoad.emplace_back(folderPath + "\\" + prefabName + ".json", prefabName);
						}
					}
				}
			}
		}
		catch (...)
		{

		}

		totalSteps = static_cast<uint32_t>(prefabsToLoad.size());

		for (const auto& prefab : prefabsToLoad)
		{
			engineContext.prefabManager.LoadPrefab(prefab.first, prefab.second);
			completedSteps++;
		}

		finished = true;
		});

}

Signal LoadManager::Update()
{
	if (finished)
	{
		completedSteps = 0;
		engineContext.textureManager.LoadMain();
		engineContext.modelManager.LoadMain();
		return Signal(SignalType::FinishedLoading);
	}
	return Signal();
}

float LoadManager::GetProgress() const
{
	return totalSteps > 0 ? float(completedSteps.load()) / float(totalSteps) : 0.0f;
}

LoadManager::LoadManager(EngineContext& engineContext) : engineContext(engineContext) {}

LoadManager::~LoadManager()
{
	if (worker.joinable())
		worker.join();
}
