module;
#include <ImGui/imgui.h>
#include <ImGui/imgui_stdlib.h>
#include "ImGui/imgui_stdlib.cpp"
#include <string>
#include <vector>
#include <functional>
module InspectorWindow;

import Entity;
import Registry;
import EditorContext;
import ID;
import Transform;
import Model;
import Texture;
import Camera;
import Script;
import ColliderBox;

using namespace Umi;

void InspectorWindow::ProcessPending()
{
    auto& registry = engineContext.registry;

    if (!pendingModelAssigns.empty());
    {
        for (auto& a : pendingModelAssigns)
        {
            if (!registry.HasComponent<Model>(a.entity))
                registry.AddComponent<Model>(a.entity, Model{});

            registry.GetComponent<Model>(a.entity).id =
                engineContext.modelManager.LoadModel(a.path);
        }
        pendingModelAssigns.clear();
    }

    if (!pendingTextureAssigns.empty())
    {
        for (auto& a : pendingTextureAssigns)
        {
            if (!registry.HasComponent<Texture>(a.entity))
                registry.AddComponent<Texture>(a.entity, Texture{});

            registry.GetComponent<Texture>(a.entity).id =
                engineContext.textureManager.LoadTexture2D(a.path);
        }
        pendingTextureAssigns.clear();
    }

    for (auto& fn : deferredActions) fn();
    deferredActions.clear();
}

void InspectorWindow::DrawID(Entity entity)
{
    if (engineContext.registry.HasComponent<ID>(entity))
    {
        auto& id = engineContext.registry.GetComponent<ID>(entity);
        {
            if (ImGui::CollapsingHeader(("Info##" + std::to_string(static_cast<int>(entity))).c_str(),
                ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::InputText("Tag", &id.tag);
            }
        }
    }
}

void InspectorWindow::DrawTransform(Entity entity)
{
    //-------------------------TRANSFORM-------------------------
    if (engineContext.registry.HasComponent<Transform>(entity))
    {
        auto& transform = engineContext.registry.GetComponent<Transform>(entity);

        if (ImGui::CollapsingHeader(("Transform##" + std::to_string(static_cast<int>(entity))).c_str(),
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Position", &transform.pos.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rot.x, 0.5f);
            ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
        }
    }
    //-----------------------------------------------------------

}

void InspectorWindow::DrawModel(Entity entity)
{
    //---------------------------MODEL---------------------------
    if (engineContext.registry.HasComponent<Model>(entity))
    {
        ImGui::PushID("ModelComponent"); // scopes the popup ID so it won't collide with other components

        bool open = ImGui::CollapsingHeader("Model",
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

        const char* dots = "...";
        float dotsW = ImGui::CalcTextSize(dots).x + ImGui::GetStyle().FramePadding.x * 2.0f;
        ImGui::SameLine(ImGui::GetContentRegionMax().x - dotsW);
        if (ImGui::SmallButton(dots))
            ImGui::OpenPopup("component_settings");

        if (ImGui::BeginPopup("component_settings"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                deferredActions.push_back([this, entity]()
                    {
                        engineContext.registry.RemoveComponent<Model>(entity);
                    });
            }
            ImGui::EndPopup();
        }

        if (open)
        {
            auto& model = engineContext.registry.GetComponent<Model>(entity);
            std::string slot = (model.id == INVALID_MODELID)
                ? "Drop model here"
                : engineContext.modelManager.GetModelData(model.id).filePath;

            ImGui::Button(slot.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("MODEL_ASSET"))
                    pendingModelAssigns.push_back({ entity, std::string((const char*)p->Data) });
                ImGui::EndDragDropTarget();
            }
        }

        ImGui::PopID();
    }
    //-----------------------------------------------------------
}

void InspectorWindow::DrawTexture(Entity entity)
{
    //--------------------------TEXTURE--------------------------
    if (engineContext.registry.HasComponent<Texture>(entity))
    {
        ImGui::PushID("TextureComponent"); // scopes the popup ID so it won't collide with other components

        bool open = ImGui::CollapsingHeader("Texture",
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

        const char* dots = "...";
        float dotsW = ImGui::CalcTextSize(dots).x + ImGui::GetStyle().FramePadding.x * 2.0f;
        ImGui::SameLine(ImGui::GetContentRegionMax().x - dotsW);
        if (ImGui::SmallButton(dots))
            ImGui::OpenPopup("component_settings");

        if (ImGui::BeginPopup("component_settings"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                deferredActions.push_back([this, entity]()
                    {
                        engineContext.registry.RemoveComponent<Texture>(entity);
                    });
            }
            ImGui::EndPopup();
        }

        if (open)
        {
            auto& texture = engineContext.registry.GetComponent<Texture>(entity);
            std::string slot = (texture.id == INVALID_TEXTUREID)
                ? "Drop Texture here"
                : engineContext.textureManager.GetTextureData(texture.id).filePath;

            ImGui::Button(slot.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                    pendingTextureAssigns.push_back({ entity, std::string((const char*)p->Data) });
                ImGui::EndDragDropTarget();
            }
        }

        ImGui::PopID();
    }
    //-----------------------------------------------------------
}

void InspectorWindow::DrawCamera(Entity entity)
{
    //---------------------------CAMERA--------------------------
    if (engineContext.registry.HasComponent<Camera>(entity))
    {
        auto& camera = engineContext.registry.GetComponent<Camera>(entity);

        if (ImGui::CollapsingHeader(("Camera##" + std::to_string(static_cast<int>(entity))).c_str(),
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("FOV", &camera.fov, 0.1f);
            ImGui::DragFloat("NearClip", &camera.nearClip, 0.1f);
            ImGui::DragFloat("FarClip", &camera.farClip, 10.0f);
        }
    }
    //-----------------------------------------------------------
}

void InspectorWindow::DrawScript(Entity entity)
{
    //--------------------------SCRIPT---------------------------
    if (engineContext.registry.HasComponent<Scripts>(entity))
    {
        ImGui::PushID("ScriptComponent"); // scopes the popup ID so it won't collide with other components

        bool open = ImGui::CollapsingHeader("Script",
            ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

        const char* dots = "...";
        float dotsW = ImGui::CalcTextSize(dots).x + ImGui::GetStyle().FramePadding.x * 2.0f;
        ImGui::SameLine(ImGui::GetContentRegionMax().x - dotsW);
        if (ImGui::SmallButton(dots))
            ImGui::OpenPopup("component_settings");

        if (ImGui::BeginPopup("component_settings"))
        {
            if (ImGui::MenuItem("Remove Component"))
            {
                deferredActions.push_back([this, entity]()
                    {
                        engineContext.registry.RemoveComponent<Scripts>(entity);
                    });
            }
            ImGui::EndPopup();
        }

        if (open)
        {
            auto& scripts = engineContext.registry.GetComponent<Scripts>(entity);
            for (auto& script : scripts.scripts)
            {
                ImGui::Button(script.name.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
            }
        }

        ImGui::PopID();
    }
    //-----------------------------------------------------------
}

void InspectorWindow::DrawColliderBox(Entity entity)
{
    //------------------------COLLIDER BOX-----------------------
    if (engineContext.registry.HasComponent<ColliderBox>(entity))
    {
        auto& collider = engineContext.registry.GetComponent<ColliderBox>(entity);

        if (ImGui::CollapsingHeader(("Collider Box##" + std::to_string(static_cast<int>(entity))).c_str(),
            ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Local Position", &collider.localPosition.x, 0.1f);
            ImGui::DragFloat3("Local Rotation", &collider.localRotation.x, 0.5f);
            ImGui::DragFloat3("Size", &collider.size.x, 0.1f);
        }
    }
    //-----------------------------------------------------------
}

void InspectorWindow::DrawAddComponentButton(Entity entity)
{
    //-----------------------ADD COMPONENT-----------------------
    if (ImGui::Button("Add Component", ImVec2(ImVec2(-FLT_MIN, 0.0f))))
    {
        ImGui::OpenPopup("add_component_popup");
    }

    if (ImGui::BeginPopup("add_component_popup"))
    {
        if (!engineContext.registry.HasComponent<Transform>(entity) && ImGui::MenuItem("Transform"))
            engineContext.registry.AddComponent<Transform>(entity, Transform{});

        if (!engineContext.registry.HasComponent<Model>(entity) && ImGui::MenuItem("Model"))
            engineContext.registry.AddComponent<Model>(entity, Model{});

        if (!engineContext.registry.HasComponent<Texture>(entity) && ImGui::MenuItem("Texture"))
            engineContext.registry.AddComponent<Texture>(entity, Texture{});

        if (!engineContext.registry.HasComponent<Camera>(entity) && ImGui::MenuItem("Camera"))
            engineContext.registry.AddComponent<Camera>(entity, Camera{});
        
        if (!engineContext.registry.HasComponent<ColliderBox>(entity) && ImGui::MenuItem("Collider Box"))
            engineContext.registry.AddComponent<ColliderBox>(entity, ColliderBox{});

        ImGui::EndPopup();
    }
    //-----------------------------------------------------------
}

void InspectorWindow::DrawAddScriptButton(Entity entity)
{
    if (ImGui::Button("Add Script")) ImGui::OpenPopup("add_script");
    if (ImGui::BeginPopup("add_script"))
    {
        for (const std::string& name : editorContext.scriptManager->GetRegisteredNames())
            if (ImGui::MenuItem(name.c_str()))
            {
                editorContext.scriptManager->AddScriptToEntity(entity, name);
            }
        ImGui::EndPopup();
    }
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

	DrawID(entity);
	DrawTransform(entity);
	DrawModel(entity);
	DrawCamera(entity);
	DrawScript(entity);
	DrawColliderBox(entity);
    
	DrawAddComponentButton(entity);
    DrawAddScriptButton(entity);

    ImGui::End();
}

InspectorWindow::InspectorWindow(EngineContext& engineContext, EditorContext& context) : engineContext(engineContext),
    editorContext(context)
{
}
