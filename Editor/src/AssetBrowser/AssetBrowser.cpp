module;
#include <ImGui/imgui.h>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>
module AssetBrowser;

import Registry;
import TextureManager;
import ModelManager;
import Texture;
import Model;
import Transform;
import ID;
import Entity;

using namespace Umi;
namespace fs = std::filesystem;

bool IsValidIdentifier(const std::string& s)
{
    if (s.empty()) return false;
    if (!(std::isalpha((unsigned char)s[0]) || s[0] == '_')) return false;
    for (char c : s)
        if (!(std::isalnum((unsigned char)c) || c == '_')) return false;
    return true;
}

static bool ContainsCI(const std::string& hay, const std::string& needle)
{
    auto it = std::search(hay.begin(), hay.end(), needle.begin(), needle.end(),
                          [](char a, char b)
                          {
                              return std::tolower((unsigned char)a) == std::tolower((unsigned char)b);
                          });
    return it != hay.end();
}

AssetBrowser::AssetType AssetBrowser::Classify(const fs::path& p) const
{
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });

    static const std::vector<std::string> tex = {".png", ".jpg", ".jpeg", ".bmp", ".tga", ".dds", ".gif"};
    static const std::vector<std::string> mdl = {".fbx", ".obj", ".gltf", ".glb", ".dae", ".blend", ".3ds", ".ply"};
    static const std::vector<std::string> lvl = {".level"};
    static const std::vector<std::string> ixx = {".ixx"};

    if (std::find(tex.begin(), tex.end(), ext) != tex.end()) return AssetType::Texture;
    if (std::find(mdl.begin(), mdl.end(), ext) != mdl.end()) return AssetType::Model;
    if (std::find(lvl.begin(), lvl.end(), ext) != lvl.end()) return AssetType::Level;
    if (std::find(ixx.begin(), ixx.end(), ext) != ixx.end()) return AssetType::Script;
    return AssetType::Other;
}

void AssetBrowser::Activate(const fs::path& fsPath, const std::string& path, const std::string& name, AssetType type)
{
    if (type == AssetType::Folder)
    {
        currentPath = fsPath;
        return;
    }
    if (type == AssetType::Texture || type == AssetType::Model || type == AssetType::Level)
        pendingLoads.push_back({path, name, type});
}

void AssetBrowser::Draw()
{
    ImGui::Begin("Assets");
    ImGui::SetWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);

    if (currentPath != rootPath)
    {
        if (ImGui::Button("<-"))
            currentPath = currentPath.parent_path();
        ImGui::SameLine();
    }
    ImGui::TextUnformatted(currentPath.generic_string().c_str());
    ImGui::SameLine();
    ImGui::SetNextItemWidth(160.0f);
    ImGui::InputTextWithHint("##search", "search", searchBuffer, sizeof(searchBuffer));
    ImGui::Separator();

    std::error_code ec;
    if (!fs::exists(currentPath, ec))
    {
        ImGui::TextWrapped("Folder not found: %s",
                           fs::absolute(currentPath, ec).generic_string().c_str());
        ImGui::End();
        return;
    }

    struct Entry
    {
        std::string name, path;
        fs::path fsPath;
        AssetType type;
        bool isDir;
    };

    std::vector<Entry> entries;
    for (auto& e : fs::directory_iterator(currentPath, ec))
    {
        Entry entry;
        entry.fsPath = e.path();
        entry.name = e.path().filename().string();
        entry.path = e.path().generic_string();
        entry.isDir = e.is_directory(ec);
        entry.type = entry.isDir ? AssetType::Folder : Classify(e.path());

        if (searchBuffer[0] != '\0' && !ContainsCI(entry.name, searchBuffer))
            continue;

        entries.push_back(std::move(entry));
    }

    std::sort(entries.begin(), entries.end(), [](const Entry& a, const Entry& b)
    {
        if (a.isDir != b.isDir) return a.isDir > b.isDir; // folders first
        return a.name < b.name;
    });

    const float cellSize = 90.0f;
    int columns = (int)(ImGui::GetContentRegionAvail().x / cellSize);
    if (columns < 1) columns = 1;
    ImGui::Columns(columns, nullptr, false);

    for (auto& entry : entries)
    {
        ImGui::PushID(entry.path.c_str());

        const char* tag = "[ ]";
        switch (entry.type)
        {
        case AssetType::Folder: tag = "[DIR]";
            break;
        case AssetType::Texture: tag = "[TEX]";
            break;
        case AssetType::Model: tag = "[MDL]";
            break;
        case AssetType::Level: tag = "[LVL]";
            break;
        case AssetType::Script: tag = "[IXX]";
            break;
        default: break;
        }

        ImGui::Button(tag, ImVec2(cellSize - 18.0f, cellSize - 18.0f));

        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Delete"))
                std::filesystem::remove_all(entry.fsPath);

            ImGui::EndPopup();
        }

        
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            Activate(entry.fsPath, entry.path, entry.name, entry.type);

        if (!entry.isDir && ImGui::BeginDragDropSource())
        {
            const char* payloadType =
                entry.type == AssetType::Model
                    ? "MODEL_ASSET"
                    : entry.type == AssetType::Texture
                    ? "TEXTURE_ASSET"
                    : entry.type == AssetType::Script
                    ? "SCRIPT_ASSET"
                    : "ASSET_PATH";
            ImGui::SetDragDropPayload(payloadType, entry.path.c_str(), entry.path.size() + 1);
            ImGui::TextUnformatted(entry.name.c_str());
            ImGui::EndDragDropSource();
        }

        ImGui::TextWrapped("%s", entry.name.c_str());
        ImGui::NextColumn();
        ImGui::PopID();
    }


    ImGui::Columns(1);
    ImGui::Columns(1);

    // Right-click empty space -> Create menu (once, at window scope, no PushID active)
    if (ImGui::BeginPopupContextWindow("##background",
        ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
    {
        if (ImGui::BeginMenu("Create"))
        {
            if (ImGui::MenuItem("Folder"))
            {
                if (!fs::create_directory(currentPath / "NewFolder"))
                {
                    for (int i = 1; i < 100; i++)
                    {
                        std::string buffer = "NewFolder" + std::to_string(i);
                        if (fs::create_directory(currentPath / buffer))
                            break;
                    }
                }
            }

            if (ImGui::MenuItem("Level"))
            {
                fs::path path = currentPath / "Level.level";
                engineContext.levelManager.CreateLevel(path.string());
            }

            if (ImGui::MenuItem("Script"))
            {
                scriptNameBuf[0] = '\0';
                createScriptStatus.clear();
                openCreateScript = true;   // just raise the flag — don't OpenPopup here
            }

            ImGui::EndMenu();
        }
        ImGui::EndPopup();
    }

    // Create Script dialog — opened AND shown at window scope, so closing the menu can't kill it
    if (openCreateScript)
    {
        ImGui::OpenPopup("Create Script");
        openCreateScript = false;
    }
    if (ImGui::BeginPopupModal("Create Script", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("New script name:");

        if (ImGui::IsWindowAppearing())
            ImGui::SetKeyboardFocusHere();

        bool submit = ImGui::InputText("##scriptname", scriptNameBuf, sizeof(scriptNameBuf),
            ImGuiInputTextFlags_EnterReturnsTrue);

        if (!createScriptStatus.empty())
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", createScriptStatus.c_str());

        if (ImGui::Button("Create") || submit)
        {
            if (CreateScriptFile(scriptNameBuf))
                ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }

    ImGui::End();
}

void AssetBrowser::ProcessPending()
{
    if (pendingLoads.empty()) return;

    auto& registry = engineContext.registry;
    for (auto& p : pendingLoads)
    {
        switch (p.type)
        {
        case AssetType::Texture:
            {
                Entity e = registry.CreateEntity();
                registry.AddComponent<ID>(e, ID{.name = p.name});
                registry.AddComponent<Transform>(e, Transform{});
                registry.AddComponent<Texture>(e, Texture{engineContext.textureManager.LoadTexture2D(p.path)});
                editorContext.selectedEntity = e;
                break;
            }
        case AssetType::Model:
            {
                Entity e = registry.CreateEntity();
                registry.AddComponent<ID>(e, ID{.name = p.name});
                registry.AddComponent<Transform>(e, Transform{});
                registry.AddComponent<Model>(e, Model{engineContext.modelManager.LoadModel(p.path)});
                editorContext.selectedEntity = e;
                break;
            }
        case AssetType::Level:
            engineContext.levelManager.LoadLevel(p.path);
            break;

        default:
            break;
        }
    }

    pendingLoads.clear();
}

bool AssetBrowser::CreateScriptFile(const std::string& name)
{
    if (!IsValidIdentifier(name))
    {
        createScriptStatus = "Name must be a valid C++ identifier.";
        return false;
    }

    std::error_code ec;
    std::filesystem::path dir = scriptsDir;
    std::filesystem::create_directories(dir, ec);

    std::filesystem::path file = dir / (name + ".ixx");
    if (std::filesystem::exists(file))
    {
        createScriptStatus = "A script named '" + name + "' already exists.";
        return false;
    }

    std::ofstream out(file);
    if (!out)
    {
        createScriptStatus = "Could not write to " + file.string();
        return false;
    }

    out << "module;\n"
        "export module Scripts." << name << ";\n"
        "\n"
        "import Engine;\n"
        "\n"
        "export namespace Umi\n"
        "{\n"
        "    class " << name << " : public BasicScript\n"
        "    {\n"
        "    public:\n"
        "        void Start() override\n" 
        "        {\n"
        "           \n"
        "        }\n"
        "        void Update() override\n"
        "        {\n"
        "           \n"
        "        }\n"
        "    };\n"
        "}\n";

    return true;
}

AssetBrowser::AssetBrowser(EngineContext& engineContext, EditorContext& editorContext, fs::path root)
    : engineContext(engineContext), editorContext(editorContext),
      rootPath(std::move(root)), currentPath(rootPath)
{
}
