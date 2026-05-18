module;
#include <ImGui/imgui.h>
#include <string>
module InspectorWindow;

import Registry;
import EditorContext;
import Transform;
import Entity;

using namespace Umi;

void InspectorWindow::Draw()
{
    ImGui::Begin("Inspector");

    Entity entity = editorContext.selectedEntity;

    if (entity == INVALID_ENTITY)
    {
        ImGui::Text("No entity selected.");
        ImGui::End();
        return;
    }

    if (registry.HasComponent<Transform>(entity))
    {
        auto& transform = registry.GetComponent<Transform>(entity);

        if (ImGui::CollapsingHeader(("Transform##" + std::to_string(static_cast<int>(entity))).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Position", &transform.pos.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rot.x, 0.1f);
            ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
        }
    }

    //if (registry.HasComponent<ModelComponent>(entity))
    //{
    //    auto& model = registry.GetComponent<ModelComponent>(entity);

    //    if (ImGui::CollapsingHeader(("Model##" + std::to_string(static_cast<int>(entity))).c_str()))
    //    {
    //        ImGui::Text("Model details here...");
    //    }
    //}

    ImGui::End();
}

InspectorWindow::InspectorWindow(Registry& registry, EditorContext& context) : registry(registry), editorContext(context) {}