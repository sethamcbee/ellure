/**
 * @file editor.cc
 */

#include "gui/editor.h"

#include "doc.h"
#include "imgui/imgui.h"
#include "gui/gui.h"

#include <cstring>

namespace Ellure
{

Editor::Editor()
{
    strcpy(text, "");
    Doc doc{"data/testdoc"};
    word_chain = ComplexWordChain{doc.get_words()};
}

void Editor::run()
{
    auto& gui = get_gui();
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    bool p_open = true;
    std::string menu_action;

    // Editor window.
#ifdef IMGUI_HAS_VIEWPORT
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
#else 
    ImGui::SetNextWindowPos(ImVec2(0.0f, gui.main_menu_size.y));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
#endif
    //ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin("Editor", &p_open,
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoResize);

    auto editor_height = ImGui::GetWindowHeight() - 4.0f * ImGui::GetTextLineHeight();
    bool written = ImGui::InputTextMultiline(
        "##editor",
        text,
        IM_ARRAYSIZE(text),
        ImVec2(-FLT_MIN, editor_height),
        flags
    );

    if (written)
    {
        dirty = true;
    }

    if (ImGui::BeginPopupContextItem("##editor"))
    {
        if (ImGui::MenuItem("Generate line"))
        {
            auto line = word_chain.get_line_bigrams();
            strcat(text, line.c_str());
            dirty = true;
        }
        if (ImGui::MenuItem("Generate multiple line options"))
        {
            menu_action = "multiple_gen";
            options.clear();
            for (size_t i = 0; i < 5; ++i)
            {
                auto line = word_chain.get_line_bigrams();
                options.push_back(line);
            }
        }

        ImGui::EndPopup();
    }

    if (menu_action == "multiple_gen")
    {
        ImGui::OpenPopup("multiple_gen");
    }
    if (ImGui::BeginPopup("multiple_gen"))
    {
        for (size_t i = 0; i < 5; ++i)
        {
            if (ImGui::MenuItem(options[i].c_str()))
            {
                strcat(text, options[i].c_str());
                options.clear();
                menu_action = "";
                dirty = true;
            }
        }

        ImGui::EndPopup();
    }

    // Finish window.
    ImGui::End();
}

}
