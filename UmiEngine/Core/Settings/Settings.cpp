#include <Settings/Settings.h>

#include <Windows.h>
#include <fstream>

#include <Json/json.hpp>
#include <iostream>
#include <sstream>
using json = nlohmann::json;

using namespace Umi;

Settings::Settings()
{
	screenResolution.width = GetSystemMetrics(SM_CXSCREEN);
	screenResolution.height = GetSystemMetrics(SM_CYSCREEN);
	Load();
}

void Settings::Load()
{
	std::ifstream inFile(JsonFilePath);
	if (inFile.is_open())
	{
		try
		{
			json j;
			inFile >> j;

			windowedFullScreenMode = j.value("windowedFullScreenMode", DEFAULT_WINDOWEDFULLSCREENMODE);

			if (screenResolution.width != j.value("screenWidth", DEFAULT_SCREEN_RESOLUTION.width) || screenResolution.height != j.value("screenHeight", DEFAULT_SCREEN_RESOLUTION.height))
			{
				if (windowedFullScreenMode || (resolution.height > screenResolution.height) || (resolution.width > screenResolution.width))
				{
					resolution = screenResolution;
				}
			}
			else
			{
				resolution.width = j.value("resolutionWidth", DEFAULT_SCREEN_RESOLUTION.width);
				resolution.height = j.value("resolutionHeight", screenResolution.height);
				if (resolution.width < MIN_RESOLUTION_WIDTH || resolution.height < MIN_RESOLUTION_HEIGHT)
				{
					resolution = screenResolution;
				}
			}

			maxFps = j.value("maxFps", DEFAULT_MAXFPS);

			masterVolume = j.value("masterVolume", DEFAULT_MASTER_VOLUME);
			if (masterVolume < 0.0f)
			{
				masterVolume = DEFAULT_MASTER_VOLUME;
			}

			fullscreenMode = j.value("fullscreenMode", DEFAULT_FULLSCREENMODE);
			vSyncEnable = j.value("vSyncEnable", DEFAULT_VSYNCENABLE);
			if (fullscreenMode && windowedFullScreenMode)
			{
				fullscreenMode = DEFAULT_FULLSCREENMODE;
				windowedFullScreenMode = DEFAULT_WINDOWEDFULLSCREENMODE;
			}

			Save();
		}
		catch (const std::exception& e)
		{
			//DBOUT("\nERROR WHEN READING SETTINGS FILE\n");
			Reset();
		}
	}
	else
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