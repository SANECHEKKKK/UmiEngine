#include "InspectorWindow.h"
#include <Registry/Registry.h>
#include <ImGui/imgui.h>
#include <ID/ID.h>
#include <EditorContext/EditorContext.h>
#include <Transform/TransformComponent.h>

void Umi::InspectorWindow::Draw()
{
    ImGui::Begin("Inspector");

    Entity entity = editorContext.selectedEntity;

    if (entity == INVALID_ENTITY)
    {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    if (registry.HasComponent<Transform3DComponent>(entity))
    {
        auto& transform = registry.GetComponent<Transform3DComponent>(entity);

        if (ImGui::CollapsingHeader("Transform"))
        {
            ImGui::DragFloat3("Position", &transform.pos.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rot.x, 0.1f);
            ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
        }
    }

    ImGui::End();
}

Umi::InspectorWindow::InspectorWindow(Registry& registry, EditorContext& context) : registry(registry), editorContext(context) {}
