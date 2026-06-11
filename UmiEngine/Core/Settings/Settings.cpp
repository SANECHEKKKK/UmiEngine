module;
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Json/json.hpp>
module Settings;

using json = nlohmann::json;

using namespace Umi;

Settings::Settings()
{
	screenResolution.width = GetSystemMetrics(SM_CXSCREEN);
	screenResolution.height = GetSystemMetrics(SM_CYSCREEN);
	Load();
}


void Settings::Save()
{
	json j;
	j["screenWidth"] = screenResolution.width;
	j["screenHeight"] = screenResolution.height;
	j["resolutionWidth"] = resolution.width;
	j["resolutionHeight"] = resolution.height;
	j["maxFps"] = maxFps;
	j["masterVolume"] = masterVolume;
	j["fullscreenMode"] = fullscreenMode;
	j["windowedFullScreenMode"] = windowedFullScreenMode;
	j["vSyncEnable"] = vSyncEnable;

	std::ofstream outFile(JsonFilePath);
	outFile << j.dump(4);
}

void Settings::Reset()
{
	resolution.width = screenResolution.width;
	resolution.height = screenResolution.height;
	maxFps = DEFAULT_MAXFPS;
	masterVolume = DEFAULT_MASTER_VOLUME;
	fullscreenMode = DEFAULT_FULLSCREENMODE;
	windowedFullScreenMode = DEFAULT_WINDOWEDFULLSCREENMODE;
	vSyncEnable = DEFAULT_VSYNCENABLE;
	Save();
}

void Settings::setResolution(int width, int height) {
	resolution.width = width;
	resolution.height = height;
	windowedFullScreenMode = false;
	fullscreenMode = false;
	Save();
}