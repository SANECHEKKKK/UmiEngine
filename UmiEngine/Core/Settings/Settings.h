#pragma once
#include <string>
#include <Resolution/Resolution.h>

namespace Umi
{
	class Settings
	{
	private:
		//--------JSON------//
		const std::string JsonFilePath = "Settings.json";
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
		Resolution resolution;
		Resolution screenResolution;
		int maxFps;
		float masterVolume;
		bool fullscreenMode;
		bool windowedFullScreenMode;
		bool vSyncEnable;
		//bool enableDebugMode = true;
		//-----------------------------------------//


		void Load();
		void Save();
		void Reset();

	public:

		//-----------------GET SET-----------------//
		const Resolution& getResolution() const noexcept { return resolution; }
		void setResolution(int width, int height);
		const Resolution& getScreenResolution() const noexcept { return screenResolution; }
		void setScreenResolution() { resolution = screenResolution; };

		inline const int& getMaxFps() const noexcept { return maxFps == 0 ? DEFAULT_MAXFPS : maxFps; }
		void setMaxFps(int fps) noexcept { maxFps = fps; }

		const float& getMasterVolume() const noexcept { return masterVolume; }
		void setMasterVolume(float volume) noexcept { masterVolume = volume; }

		const bool& isFullScreenMode() const noexcept { return fullscreenMode; }
		void setFullScreenMode(bool fullscreen) noexcept { fullscreenMode = fullscreen; }

		const bool& isWindowedFullScreenMode() const noexcept { return windowedFullScreenMode; }
		void setWindowedFullScreenMode(bool windowedFullScreen) noexcept { if (windowedFullScreen) setScreenResolution(); windowedFullScreenMode = windowedFullScreen; Save(); }

		inline const bool& isVSyncEnabled() const noexcept { return vSyncEnable; }
		void setVSyncEnabled(bool enabled) noexcept { vSyncEnable = enabled; }
		void ToggleVSync() noexcept { vSyncEnable = !vSyncEnable; Save(); }
		//-----------------------------------------//



		Settings();
		Settings(const Settings&) = delete;					//Prevent copying
		Settings& operator=(const Settings&) = delete;		//Prevent assignment
		Settings(Settings&&) = delete;						//Prevent move construction
		Settings& operator=(Settings&&) = delete;			//Prevent move assignment

		~Settings() = default;								//Default destructor
	};
}