#include "HierarchyWindow.h"
#include <Registry/Registry.h>
#include <ImGui/imgui.h>
#include <ID/ID.h>
#include <EditorContext/EditorContext.h>
#include <Transform/TransformComponent.h>

void Umi::HierarchyWindow::CreateEntity()
{
	Entity e = registry.CreateEntity();
	registry.AddComponent(e, IDComponent{ .id = e, .name = "Entity" + std::to_string(e) });
	registry.AddComponent(e, Transform3DComponent{});
}

void Umi::HierarchyWindow::Draw()
{

	ImGui::Begin("Hierarchy");
	ImGui::SetWindowSize(ImVec2(200, 400), ImGuiCond_FirstUseEver);

	for (auto entity : registry.View<IDComponent>())
	{
		auto& id = registry.GetComponent<IDComponent>(entity);

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
