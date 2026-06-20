module;
#include <ImGui/imgui.h>
module TopBar;

using namespace Umi;

void TopBar::Draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                engineContext.levelManager.SaveLevel();
            }
                        
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            // if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            // if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {} // Disabled item
            // ImGui::Separator();
            // if (ImGui::MenuItem("Cut", "Ctrl+X")) {}
            // if (ImGui::MenuItem("Copy", "Ctrl+C")) {}
            // if (ImGui::MenuItem("Paste", "Ctrl+V")) {}
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

