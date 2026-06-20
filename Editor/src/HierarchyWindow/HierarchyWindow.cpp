module;
#include <ImGui/imgui.h>
#include <string>
module HierarchyWindow;

import Registry;
import EditorContext;

import Entity;
import Transform;
import ID;

void Umi::HierarchyWindow::CreateEntity()
{
	Entity e = registry.CreateEntity();
	registry.AddComponent(e, ID{ .name = "Entity" + std::to_string(static_cast<uint32_t>(e)) });
	registry.AddComponent(e, Transform{});
}

void Umi::HierarchyWindow::Draw()
{
	ImGui::Begin("Hierarchy");
	ImGui::SetWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);

	for (auto entity : registry.View<ID>())
	{
		auto& id = registry.GetComponent<ID>(entity);
		
		if (id.name == "EDITOR_CAMERA")
			continue;

		bool isSelected = (editorContext.selectedEntity == entity);
		bool isRenaming = (entityToRename == entity && renamingActive);

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
					id.name = renameBuffer;
				entityToRename = Entity{ INVALID_ENTITY };
				renamingActive = false;
			}

			if (ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				entityToRename = Entity{ INVALID_ENTITY };
				renamingActive = false;
			}

		}
		else
		{
			if (ImGui::Selectable(id.name.c_str(), isSelected))
				editorContext.selectedEntity = entity;

			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Rename"))
				{
					entityToRename = entity;
					renamingActive = true;
					renamingJustStarted = true;
					strncpy_s(renameBuffer, id.name.c_str(), sizeof(renameBuffer));
				}
				if (ImGui::MenuItem("Delete"))
					registry.DestroyEntity(entity);

				ImGui::EndPopup();
			}
		}
	}

	if (ImGui::BeginPopupContextWindow("##background", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
	{
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Entity"))
				CreateEntity();
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
}

Umi::HierarchyWindow::HierarchyWindow(Registry& registry, EditorContext& editorContext) : registry(registry), editorContext(editorContext) {}