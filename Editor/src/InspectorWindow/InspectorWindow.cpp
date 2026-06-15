module;
#include <ImGui/imgui.h>
#include <string>
module InspectorWindow;

import Entity;
import Registry;
import EditorContext;
import Transform;
import Model;
import Texture;

using namespace Umi;

void InspectorWindow::ProcessPending()
{
    if (pendingAssigns.empty()) return;

    auto& registry = engineContext.registry;
    for (auto& a : pendingAssigns)
    {
        if (!registry.HasComponent<Model>(a.entity))
            registry.AddComponent<Model>(a.entity, Model{});

        registry.GetComponent<Model>(a.entity).id =
            engineContext.modelManager.LoadModel(a.path);
    }
    pendingAssigns.clear();
}

void InspectorWindow::Draw()
{
    auto& registry = engineContext.registry;

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
    
    if (registry.HasComponent<Model>(entity))
    {
        auto& model = registry.GetComponent<Model>(entity);

        if (ImGui::CollapsingHeader("Model", ImGuiTreeNodeFlags_DefaultOpen))
        {
            std::string slot = (model.id == INVALID_MODELID)
                ? "Drop model here"
                : engineContext.modelManager.GetModelData(model.id).filePath;

            ImGui::Button(slot.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("MODEL_ASSET"))
                {
                    const char* path = static_cast<const char*>(p->Data);
                    pendingAssigns.push_back({ entity, std::string(path) });
                }
                ImGui::EndDragDropTarget();
            }
        }
    }



    if (ImGui::Button("Add Component", ImVec2(ImVec2(-FLT_MIN, 0.0f))))
    {
        ImGui::OpenPopup("add_component_popup");
    }

    if (ImGui::BeginPopup("add_component_popup"))
    {
        if (!registry.HasComponent<Transform>(entity) && ImGui::MenuItem("Transform"))
            registry.AddComponent<Transform>(entity, Transform{});

        if (!registry.HasComponent<Model>(entity) && ImGui::MenuItem("Model"))
			registry.AddComponent<Model>(entity, Model{});
		
        if (!registry.HasComponent<Texture>(entity) && ImGui::MenuItem("Texture"))
			registry.AddComponent<Texture>(entity, Texture{});

        //if (!registry.HasComponent<Camera>(entity) && ImGui::MenuItem("Camera"))
            //registry.AddComponent<Camera>(entity, Camera{});

        ImGui::EndPopup();
    }


    ImGui::End();
}

InspectorWindow::InspectorWindow(EngineContext& engineContext, EditorContext& context) : engineContext(engineContext), editorContext(context) {}