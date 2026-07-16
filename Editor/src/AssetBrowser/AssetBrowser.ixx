module;
#include <filesystem>
#include <string>
#include <vector>
export module AssetBrowser;

import EngineContext;
import EditorContext;

export namespace Umi
{
	class AssetBrowser
	{
	private:
		enum class AssetType { Folder, Texture, Model, Level, Script, Other };

		EngineContext& engineContext;
		EditorContext& editorContext;

		std::filesystem::path rootPath;
		std::filesystem::path currentPath;
		char searchBuffer[128]{};

		struct PendingLoad { std::string path; std::string name; AssetType type; };
		std::vector<PendingLoad> pendingLoads;

		AssetType Classify(const std::filesystem::path& p) const;
		void Activate(const std::filesystem::path& fsPath, const std::string& path,
			const std::string& name, AssetType type);

		std::string scriptsDir = "../Game/Scripts";

		char scriptNameBuf[64] = "";
		std::string createScriptStatus;

		bool openCreateScript = false;

		bool CreateScriptFile(const std::string& name);
		
	public:
		void Draw();
		void ProcessPending();   // call only when the command list is NOT mid-frame

		AssetBrowser(EngineContext& engineContext, EditorContext& editorContext,
			std::filesystem::path root = "Assets");
	};
}