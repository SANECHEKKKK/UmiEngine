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

		bool isSelected = (editorContext.selectedEntity == entity);
		if (ImGui::Selectable(id.name.c_str(), isSelected))
			editorContext.selectedEntity = entity;
	}

	if (ImGui::BeginPopupContextWindow())
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