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

#pragma region ID
    if (registry.HasComponent<ID>(entity))
    {
        auto& id = registry.GetComponent<ID>(entity);
        {
            if (ImGui::CollapsingHeader(("Info##" + std::to_string(static_cast<int>(entity))).c_str(),
                                        ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::InputText("Tag", &id.tag);
            }
        }
    }
#pragma endregion ID

#pragma region TRANSFORM
    //-------------------------TRANSFORM-------------------------
    if (registry.HasComponent<Transform>(entity))
    {
        auto& transform = registry.GetComponent<Transform>(entity);

        if (ImGui::CollapsingHeader(("Transform##" + std::to_string(static_cast<int>(entity))).c_str(),
                                    ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Position", &transform.pos.x, 0.1f);
            ImGui::DragFloat3("Rotation", &transform.rot.x, 0.5f);
            ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
        }
    }
    //-----------------------------------------------------------
#pragma endregion TRANSFORM

#pragma region MODEL
    //---------------------------MODEL---------------------------
    if (registry.HasComponent<Model>(entity))
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
            auto& model = registry.GetComponent<Model>(entity);
            std::string slot = (model.id == INVALID_MODELID)
                                   ? "Drop model here"
                                   : engineContext.modelManager.GetModelData(model.id).filePath;

            ImGui::Button(slot.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("MODEL_ASSET"))
                    pendingModelAssigns.push_back({entity, std::string((const char*)p->Data)});
                ImGui::EndDragDropTarget();
            }
        }

        ImGui::PopID();
    }
    //-----------------------------------------------------------
#pragma endregion MODEL

#pragma region TEXTURE
    //--------------------------TEXTURE--------------------------
    if (registry.HasComponent<Texture>(entity))
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
            auto& texture = registry.GetComponent<Texture>(entity);
            std::string slot = (texture.id == INVALID_TEXTUREID)
                                   ? "Drop Texture here"
                                   : engineContext.textureManager.GetTextureData(texture.id).filePath;

            ImGui::Button(slot.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
                    pendingTextureAssigns.push_back({entity, std::string((const char*)p->Data)});
                ImGui::EndDragDropTarget();
            }
        }

        ImGui::PopID();
    }
    //-----------------------------------------------------------
#pragma endregion TEXTURE

#pragma region CAMERA
    //-------------------------TRANSFORM-------------------------
    if (registry.HasComponent<Camera>(entity))
    {
        auto& camera = registry.GetComponent<Camera>(entity);

        if (ImGui::CollapsingHeader(("Camera##" + std::to_string(static_cast<int>(entity))).c_str(),
                                    ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat("FOV", &camera.fov, 0.1f);
            ImGui::DragFloat("NearClip", &camera.nearClip, 0.1f);
            ImGui::DragFloat("FarClip", &camera.farClip, 10.0f);
        }
    }
    //-----------------------------------------------------------
#pragma endregion CAMERA
    
#pragma region SCRIPT
    //--------------------------SCRIPT---------------------------
    // if (registry.HasComponent<Scripts>(entity))
    // {
    //     ImGui::PushID("ScriptComponent"); // scopes the popup ID so it won't collide with other components
    //
    //     bool open = ImGui::CollapsingHeader("Script",
    //                                         ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);
    //
    //     const char* dots = "...";
    //     float dotsW = ImGui::CalcTextSize(dots).x + ImGui::GetStyle().FramePadding.x * 2.0f;
    //     ImGui::SameLine(ImGui::GetContentRegionMax().x - dotsW);
    //     if (ImGui::SmallButton(dots))
    //         ImGui::OpenPopup("component_settings");
    //
    //     if (ImGui::BeginPopup("component_settings"))
    //     {
    //         if (ImGui::MenuItem("Remove Component"))
    //         {
    //             deferredActions.push_back([this, entity]()
    //             {
    //                 engineContext.registry.RemoveComponent<Scripts>(entity);
    //             });
    //         }
    //         ImGui::EndPopup();
    //     }
    //
    //     if (open)
    //     {
    //         auto& scripts = registry.GetComponent<Scripts>(entity);
    //         for (size_t i = 0; i < scripts.scripts.size(); ++i)
    //         {
    //             std::string slot = (scripts.paths[i])
    //                                    ? "Drop Texture here"
    //                                    : engineContext.textureManager.GetTextureData(texture.id).filePath;
    //
    //             ImGui::Button(slot.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
    //
    //             if (ImGui::BeginDragDropTarget())
    //             {
    //                 if (const ImGuiPayload* p = ImGui::AcceptDragDropPayload("TEXTURE_ASSET"))
    //                     pendingTextureAssigns.push_back({entity, std::string((const char*)p->Data)});
    //                 ImGui::EndDragDropTarget();
    //             }
    //         }
    //     }
    //
    //     ImGui::PopID();
    // }
    //-----------------------------------------------------------
#pragma endregion SCRIPT


    //-----------------------ADD COMPONENT-----------------------
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

        if (!registry.HasComponent<Camera>(entity) && ImGui::MenuItem("Camera"))
            registry.AddComponent<Camera>(entity, Camera{});

        // if (!registry.HasComponent<Scripts>(entity) && ImGui::MenuItem("Script"))
        // registry.AddComponent<Scripts>(entity, Scripts{});

        ImGui::EndPopup();
    }
    //-----------------------------------------------------------


    ImGui::End();
}

InspectorWindow::InspectorWindow(EngineContext& engineContext, EditorContext& context) : engineContext(engineContext),
    editorContext(context)
{
}
