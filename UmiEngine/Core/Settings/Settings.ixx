module;
#include <string>
#include <Windows.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Json/json.hpp>

#include <EngineApi/EngineApi.h>
import Resolution;

export module Settings;


using json = nlohmann::json;

export namespace Umi
{
	class ENGINE_API Settings
	{
	private:
		//--------JSON------//
		inline static const std::string JsonFilePath = "Settings.json";
		//------------------//

		//-------------DEFAULT SETTINGS------------//
		static constexpr Resolution DEFAULT_RESOLUTION = { 1920, 1080 };
		static constexpr Resolution DEFAULT_SCREEN_RESOLUTION = { 1920, 1080 };
		static constexpr int DEFAULT_MAXFPS = 60;
		static constexpr float DEFAULT_MASTER_VOLUME = 0.5f;
		static constexpr bool DEFAULT_FULLSCREENMODE = false;
		static constexpr bool DEFAULT_WINDOWEDFULLSCREENMODE = false;
		static constexpr bool DEFAULT_VSYNCENABLE = false;

		static constexpr int MIN_RESOLUTION_WIDTH = 500;
		static constexpr int MIN_RESOLUTION_HEIGHT = 500;
		//-----------------------------------------//


		//-----------------VARIABLES---------------//
		inline static Resolution resolution;
		inline static Resolution screenResolution;
		inline static int maxFps;
		inline static float masterVolume;
		inline static bool fullscreenMode;
		inline static bool windowedFullScreenMode;
		inline static bool vSyncEnable;
		//bool enableDebugMode = true;
		//-----------------------------------------//


		static void Load()
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
		};
		static void Save();
		static void Reset();

	public:

		//-----------------GET SET-----------------//
		static const Resolution& getResolution() noexcept { return resolution; }
		static void setResolution(int width, int height);
		static const Resolution& getScreenResolution() noexcept { return screenResolution; }
		static void setScreenResolution() { resolution = screenResolution; };

		static inline const int& getMaxFps() noexcept { return maxFps == 0 ? DEFAULT_MAXFPS : maxFps; }
		static void setMaxFps(int fps) noexcept { maxFps = fps; }

		static const float& getMasterVolume() noexcept { return masterVolume; }
		static void setMasterVolume(float volume) noexcept { masterVolume = volume; }

		static const bool& isFullScreenMode() noexcept { return fullscreenMode; }
		static void setFullScreenMode(bool fullscreen) noexcept { fullscreenMode = fullscreen; }

		static const bool& isWindowedFullScreenMode() noexcept { return windowedFullScreenMode; }
		static void setWindowedFullScreenMode(bool windowedFullScreen) noexcept { if (windowedFullScreen) setScreenResolution(); windowedFullScreenMode = windowedFullScreen; Save(); }

		inline const bool& isVSyncEnabled() const noexcept { return vSyncEnable; }
		void setVSyncEnabled(bool enabled) noexcept { vSyncEnable = enabled; }
		static  void ToggleVSync() noexcept { vSyncEnable = !vSyncEnable; Save(); }
		//-----------------------------------------//



		Settings();
		Settings(const Settings&) = delete;					//Prevent copying
		Settings& operator=(const Settings&) = delete;		//Prevent assignment
		Settings(Settings&&) = delete;						//Prevent move construction
		Settings& operator=(Settings&&) = delete;			//Prevent move assignment

		~Settings() = default;								//Default destructor
	};
}