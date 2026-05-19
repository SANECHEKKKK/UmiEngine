module;
#include <filesystem>
#include <algorithm>

#include <ImGui/imgui.h>
module AssetBrowser;

using namespace Umi;

AssetBrowser::AssetBrowser(const std::filesystem::path& rootPath)
	: rootPath(rootPath)
{
	NavigateTo(rootPath);
}

// -------------------------------------------------------------------------
// Main Draw
// -------------------------------------------------------------------------

void AssetBrowser::Draw()
{
	ImGui::Begin("Asset Browser");
	ImGui::SetWindowSize(ImVec2(800, 500), ImGuiCond_FirstUseEver);

	DrawToolbar();
	ImGui::Separator();

	ImGui::BeginChild("##tree", ImVec2(200, 0), true);
	DrawFolderTree(rootPath);
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("##content", ImVec2(0, 0), true);
	DrawContent();
	ImGui::EndChild();

	ImGui::End();
}

// -------------------------------------------------------------------------
// Toolbar
// -------------------------------------------------------------------------

void AssetBrowser::DrawToolbar()
{
	// Back / Forward
	bool canGoBack = historyIndex > 0;
	bool canGoForward = historyIndex < (int)navHistory.size() - 1;

	if (!canGoBack) ImGui::BeginDisabled();
	if (ImGui::Button("<##back"))
	{
		historyIndex--;
		currentPath = navHistory[historyIndex];
		RefreshDirectory();
	}
	if (!canGoBack) ImGui::EndDisabled();

	ImGui::SameLine();

	if (!canGoForward) ImGui::BeginDisabled();
	if (ImGui::Button(">##fwd"))
	{
		historyIndex++;
		currentPath = navHistory[historyIndex];
		RefreshDirectory();
	}
	if (!canGoForward) ImGui::EndDisabled();

	ImGui::SameLine();

	// Breadcrumb — clickable segments
	std::filesystem::path accumulated = rootPath.parent_path();
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
	bool first = true;
	for (auto& part : std::filesystem::relative(currentPath, rootPath.parent_path()))
	{
		accumulated /= part;
		if (!first) ImGui::SameLine(0, 2), ImGui::TextUnformatted("/"), ImGui::SameLine(0, 2);
		first = false;

		std::string label = part.string() + "##bc";
		if (ImGui::SmallButton(label.c_str()))
			NavigateTo(accumulated);
	}
	ImGui::PopStyleColor();

	ImGui::SameLine(0, 20);

	// Search
	ImGui::SetNextItemWidth(180.0f);
	if (ImGui::InputText("##search", searchBuffer, sizeof(searchBuffer)))
		ApplyFilter();

	ImGui::SameLine();

	// View toggle
	if (ImGui::Button(viewMode == ViewMode::Grid ? " List " : " Grid "))
		viewMode = (viewMode == ViewMode::Grid) ? ViewMode::List : ViewMode::Grid;

	// Thumbnail size slider (grid only)
	if (viewMode == ViewMode::Grid)
	{
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::SliderFloat("##size", &thumbnailSize, 32.0f, 128.0f, "%.0fpx");
	}
}

// -------------------------------------------------------------------------
// Folder Tree (left panel)
// -------------------------------------------------------------------------

void AssetBrowser::DrawFolderTree(const std::filesystem::path& path)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
		| ImGuiTreeNodeFlags_SpanFullWidth;

	if (path == currentPath)
		flags |= ImGuiTreeNodeFlags_Selected;

	// Leaf if no subdirectories
	bool hasSubDirs = false;
	std::error_code ec;
	for (auto& e : std::filesystem::directory_iterator(path, ec))
		if (e.is_directory()) { hasSubDirs = true; break; }
	if (!hasSubDirs)
		flags |= ImGuiTreeNodeFlags_Leaf;

	// Always expand root
	if (path == rootPath)
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);

	std::string label = (path == rootPath) ? "Assets" : path.filename().string();
	bool open = ImGui::TreeNodeEx(path.string().c_str(), flags, "%s", label.c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
		NavigateTo(path);

	if (open)
	{
		// Sorted subdirectories
		std::vector<std::filesystem::path> subdirs;
		std::error_code ec2;
		for (auto& e : std::filesystem::directory_iterator(path, ec2))
			if (e.is_directory())
				subdirs.push_back(e.path());
		std::sort(subdirs.begin(), subdirs.end());

		for (auto& sub : subdirs)
			DrawFolderTree(sub);

		ImGui::TreePop();
	}
}

// -------------------------------------------------------------------------
// Content area
// -------------------------------------------------------------------------

void AssetBrowser::DrawContent()
{
	if (viewMode == ViewMode::Grid)
		DrawGridView();
	else
		DrawListView();

	// Right-click on empty space
	if (ImGui::BeginPopupContextWindow("##bgctx",
		ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
	{
		DrawContextMenu(nullptr);
		ImGui::EndPopup();
	}
}

void AssetBrowser::DrawGridView()
{
	float cellSize = thumbnailSize + 24.0f;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int   columns = std::max(1, (int)(panelWidth / cellSize));

	ImGui::Columns(columns, nullptr, false);

	for (auto* entry : filteredEntries)
	{
		ImGui::PushID(entry->path.string().c_str());

		bool isSelected = (selectedEntry == entry);
		bool isRenaming = (renamingEntry == entry);

		// --- Thumbnail / icon button ---
		ImTextureID thumb = GetOrLoadThumbnail(*entry);
		if (thumb)
			ImGui::ImageButton("##thumb", thumb, ImVec2(thumbnailSize, thumbnailSize));
		else
			ImGui::Button(GetAssetTypeIcon(entry->type), ImVec2(thumbnailSize, thumbnailSize));

		if (isSelected)
		{
			// Selection outline
			auto* dl = ImGui::GetWindowDrawList();
			dl->AddRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(),
				IM_COL32(255, 165, 0, 255), 3.0f, 0, 2.0f);
		}

		if (ImGui::IsItemClicked())
			selectedEntry = entry;

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			if (entry->isDirectory) NavigateTo(entry->path);

		// Drag & drop source
		if (!isRenaming && ImGui::BeginDragDropSource())
		{
			std::string pathStr = entry->path.string();
			ImGui::SetDragDropPayload(DRAG_DROP_PAYLOAD, pathStr.c_str(), pathStr.size() + 1);
			ImGui::Text("[%s] %s", GetAssetTypeName(entry->type), entry->name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem("##ictx"))
		{
			DrawContextMenu(entry);
			ImGui::EndPopup();
		}

		// --- Name label or rename input ---
		float colWidth = ImGui::GetColumnWidth() - 4.0f;
		if (isRenaming)
		{
			if (renamingJustStarted)
			{
				ImGui::SetKeyboardFocusHere();
				renamingJustStarted = false;
			}

			ImGui::SetNextItemWidth(colWidth);
			bool confirm = ImGui::InputText("##rename", renameBuffer, sizeof(renameBuffer),
				ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

			if (confirm || ImGui::IsItemDeactivated())
			{
				if (renameBuffer[0] != '\0')
				{
					auto newPath = entry->path.parent_path() / renameBuffer;
					std::error_code ec;
					std::filesystem::rename(entry->path, newPath, ec);
					RefreshDirectory();
				}
				renamingEntry = nullptr;
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				renamingEntry = nullptr;
		}
		else
		{
			// Truncate name to column width
			std::string display = entry->name;
			while (display.size() > 3 &&
				ImGui::CalcTextSize((display + "...").c_str()).x > colWidth)
				display.pop_back();
			if (display != entry->name) display += "...";

			ImGui::TextUnformatted(display.c_str());
		}

		ImGui::NextColumn();
		ImGui::PopID();
	}

	ImGui::Columns(1);
}

void AssetBrowser::DrawListView()
{
	ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable
		| ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY;

	if (!ImGui::BeginTable("##list", 3, flags)) return;

	ImGui::TableSetupScrollFreeze(0, 1);
	ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_DefaultSort);
	ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 80.0f);
	ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 80.0f);
	ImGui::TableHeadersRow();

	for (auto* entry : filteredEntries)
	{
		ImGui::PushID(entry->path.string().c_str());
		ImGui::TableNextRow();

		bool isSelected = (selectedEntry == entry);
		bool isRenaming = (renamingEntry == entry);

		// Selectable spanning all columns
		ImGui::TableSetColumnIndex(0);
		ImGuiSelectableFlags selFlags = ImGuiSelectableFlags_SpanAllColumns
			| ImGuiSelectableFlags_AllowOverlap;
		if (ImGui::Selectable("##row", isSelected, selFlags))
			selectedEntry = entry;

		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
			if (entry->isDirectory) NavigateTo(entry->path);

		if (!isRenaming && ImGui::BeginDragDropSource())
		{
			std::string pathStr = entry->path.string();
			ImGui::SetDragDropPayload(DRAG_DROP_PAYLOAD, pathStr.c_str(), pathStr.size() + 1);
			ImGui::Text("[%s] %s", GetAssetTypeName(entry->type), entry->name.c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginPopupContextItem("##ictx"))
		{
			DrawContextMenu(entry);
			ImGui::EndPopup();
		}

		// Name column: rename input or label
		ImGui::SameLine();
		if (isRenaming)
		{
			if (renamingJustStarted)
			{
				ImGui::SetKeyboardFocusHere();
				renamingJustStarted = false;
			}

			ImGui::SetNextItemWidth(-1);
			bool confirm = ImGui::InputText("##rename", renameBuffer, sizeof(renameBuffer),
				ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

			if (confirm || ImGui::IsItemDeactivated())
			{
				if (renameBuffer[0] != '\0')
				{
					auto newPath = entry->path.parent_path() / renameBuffer;
					std::error_code ec;
					std::filesystem::rename(entry->path, newPath, ec);
					RefreshDirectory();
				}
				renamingEntry = nullptr;
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
				renamingEntry = nullptr;
		}
		else
		{
			ImGui::Text("%s  %s", GetAssetTypeIcon(entry->type), entry->name.c_str());
		}

		// Type column
		ImGui::TableSetColumnIndex(1);
		ImGui::TextUnformatted(GetAssetTypeName(entry->type));

		// Size column
		ImGui::TableSetColumnIndex(2);
		if (!entry->isDirectory)
		{
			std::error_code ec;
			auto bytes = std::filesystem::file_size(entry->path, ec);
			if (!ec)
			{
				if (bytes < 1024)              ImGui::Text("%llu B", (unsigned long long)bytes);
				else if (bytes < 1024 * 1024)       ImGui::Text("%.1f KB", bytes / 1024.0f);
				else                                ImGui::Text("%.1f MB", bytes / (1024.0f * 1024.0f));
			}
		}

		ImGui::PopID();
	}

	ImGui::EndTable();
}

// -------------------------------------------------------------------------
// Context menu
// -------------------------------------------------------------------------

void AssetBrowser::DrawContextMenu(AssetEntry* entry)
{
	if (entry)
	{
		if (entry->isDirectory && ImGui::MenuItem("Open"))
			NavigateTo(entry->path);

		if (ImGui::MenuItem("Rename"))
		{
			renamingEntry = entry;
			renamingJustStarted = true;
			strncpy_s(renameBuffer, entry->name.c_str(), sizeof(renameBuffer));
			ImGui::CloseCurrentPopup();
		}

		if (ImGui::MenuItem("Delete"))
		{
			std::error_code ec;
			std::filesystem::remove_all(entry->path, ec);
			if (selectedEntry == entry) selectedEntry = nullptr;
			if (renamingEntry == entry) renamingEntry = nullptr;
			RefreshDirectory();
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Show in Explorer"))
		{
#ifdef _WIN32
			std::string cmd = "explorer /select,\"" + entry->path.string() + "\"";
			system(cmd.c_str());
#endif
		}
	}
	else
	{
		if (ImGui::BeginMenu("New"))
		{
			if (ImGui::MenuItem("Folder"))
			{
				std::error_code ec;
				std::filesystem::create_directory(currentPath / "New Folder", ec);
				RefreshDirectory();
			}
			ImGui::EndMenu();
		}
		if (ImGui::MenuItem("Refresh"))
			RefreshDirectory();
	}
}

// -------------------------------------------------------------------------
// Navigation & refresh
// -------------------------------------------------------------------------

void AssetBrowser::NavigateTo(const std::filesystem::path& path)
{
	// Discard any forward history
	if (historyIndex < (int)navHistory.size() - 1)
		navHistory.erase(navHistory.begin() + historyIndex + 1, navHistory.end());

	navHistory.push_back(path);
	historyIndex = (int)navHistory.size() - 1;
	currentPath = path;
	RefreshDirectory();
}

void AssetBrowser::RefreshDirectory()
{
	entries.clear();

	std::error_code ec;
	std::vector<std::filesystem::directory_entry> raw;
	for (auto& e : std::filesystem::directory_iterator(currentPath, ec))
		raw.push_back(e);

	// Folders first, then alphabetical
	std::sort(raw.begin(), raw.end(), [](const auto& a, const auto& b) {
		if (a.is_directory() != b.is_directory())
			return a.is_directory() > b.is_directory();
		return a.path().filename() < b.path().filename();
		});

	for (auto& e : raw)
	{
		AssetEntry asset;
		asset.path = e.path();
		asset.name = e.path().filename().string();
		asset.isDirectory = e.is_directory();
		asset.type = GetAssetType(e.path());
		entries.push_back(std::move(asset));
	}

	ApplyFilter();
}

void AssetBrowser::ApplyFilter()
{
	filteredEntries.clear();
	std::string filter = searchBuffer;
	std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);

	for (auto& entry : entries)
	{
		if (filter.empty()) { filteredEntries.push_back(&entry); continue; }
		std::string name = entry.name;
		std::transform(name.begin(), name.end(), name.begin(), ::tolower);
		if (name.find(filter) != std::string::npos)
			filteredEntries.push_back(&entry);
	}
}

// -------------------------------------------------------------------------
// Helpers
// -------------------------------------------------------------------------

AssetType AssetBrowser::GetAssetType(const std::filesystem::path& path)
{
	if (std::filesystem::is_directory(path)) return AssetType::Folder;

	auto ext = path.extension().string();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

	if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".tga" || ext == ".bmp")
		return AssetType::Texture;
	if (ext == ".obj" || ext == ".fbx" || ext == ".gltf" || ext == ".glb")
		return AssetType::Mesh;
	if (ext == ".wav" || ext == ".mp3" || ext == ".ogg" || ext == ".flac")
		return AssetType::Audio;
	if (ext == ".lua" || ext == ".cs" || ext == ".py")
		return AssetType::Script;
	if (ext == ".scene")
		return AssetType::Scene;

	return AssetType::Unknown;
}

ImTextureID AssetBrowser::GetOrLoadThumbnail(AssetEntry& entry)
{
	if (entry.thumbnail) return entry.thumbnail;
	if (onRequestThumbnail)
		entry.thumbnail = onRequestThumbnail(entry.path);
	return entry.thumbnail;
}

const char* AssetBrowser::GetAssetTypeIcon(AssetType type)
{
	switch (type)
	{
	case AssetType::Folder:  return "[D]";
	case AssetType::Texture: return "[I]";
	case AssetType::Mesh:    return "[M]";
	case AssetType::Audio:   return "[A]";
	case AssetType::Script:  return "[S]";
	case AssetType::Scene:   return "[SC]";
	default:                 return "[?]";
	}
}

const char* AssetBrowser::GetAssetTypeName(AssetType type)
{
	switch (type)
	{
	case AssetType::Folder:  return "Folder";
	case AssetType::Texture: return "Texture";
	case AssetType::Mesh:    return "Mesh";
	case AssetType::Audio:   return "Audio";
	case AssetType::Script:  return "Script";
	case AssetType::Scene:   return "Scene";
	default:                 return "Unknown";
	}
}
