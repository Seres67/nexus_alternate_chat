#include <globals.hpp>
#include <gui.hpp>
#include <imgui/imgui.h>
#include <settings.hpp>

bool tmp_open = true;
void render_window()
{
    ImGui::SetNextWindowPos(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    if (ImGui::Begin("Template##TemplateMainWindow", &tmp_open, flags)) {
        ImGui::Text("Hello from template!");
        ImGui::End();
    }
}
 
void render_options()
{
}
