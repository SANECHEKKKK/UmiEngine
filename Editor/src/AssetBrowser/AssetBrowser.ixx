module;
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

#include <ImGui/imgui.h>
export module AssetBrowser;

export namespace Umi
{
    enum class AssetType { Unknown, Texture, Mesh, Audio, Script, Scene, Folder };

    struct AssetEntry
    {
        std::filesystem::path path;
        std::string name;
        AssetType type;
        bool isDirectory;
        ImTextureID thumbnail;
    };

    class AssetBrowser
    {
    public:
        explicit AssetBrowser(const std::filesystem::path& rootPath);

        void Draw();

        // Plug in your texture loader here — return nullptr if no thumbnail available
        std::function<ImTextureID(const std::filesystem::path&)> onRequestThumbnail;

        static constexpr const char* DRAG_DROP_PAYLOAD = "ASSET_PATH";

    private:
        void DrawToolbar();
        void DrawFolderTree(const std::filesystem::path& path);
        void DrawContent();
        void DrawGridView();
        void DrawListView();
        void DrawContextMenu(AssetEntry* entry); // nullptr = background context menu

        void NavigateTo(const std::filesystem::path& path);
        void RefreshDirectory();
        void ApplyFilter();

        AssetType       GetAssetType(const std::filesystem::path& path);
        ImTextureID     GetOrLoadThumbnail(AssetEntry& entry);
        const char* GetAssetTypeIcon(AssetType type);
        const char* GetAssetTypeName(AssetType type);

    private:
        enum class ViewMode { Grid, List };

        std::filesystem::path            rootPath;
        std::filesystem::path            currentPath;
        std::vector<std::filesystem::path> navHistory;
        int                              historyIndex = -1;

        std::vector<AssetEntry>          entries;
        std::vector<AssetEntry*>         filteredEntries;

        char        searchBuffer[256] = {};
        ViewMode    viewMode = ViewMode::Grid;
        float       thumbnailSize = 64.0f;

        AssetEntry* selectedEntry = nullptr;
        AssetEntry* renamingEntry = nullptr;
        char        renameBuffer[256] = {};
        bool        renamingJustStarted = false;
    };
}