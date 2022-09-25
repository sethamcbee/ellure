/**
 * @file editor.cc
 */

#include "gui/editor.h"

#include "doc.h"
#include "imgui_custom.h"
#include "gui/gui.h"

#include <cstring>

namespace Ellure
{

void string_insert(char* dest, const char* src)
{
    std::string dest_backup{dest};
    auto src_len = strlen(src);
    strcpy(dest, src);
    strcat(dest + src_len, dest_backup.c_str());
}

static int editor_callback(ImGuiInputTextCallbackData* data)
{
    auto& gui = get_gui();
    auto& editor = gui.editor;
    editor.callback = *data;
    return 0;
}

Editor::Editor()
{
    strcpy(text, "");
    Doc doc{"data/testdoc"};
    word_chain = ComplexWordChain{doc.get_words()};
}

void Editor::run()
{
    auto& gui = get_gui();
    ImGuiInputTextFlags flags =
        ImGuiInputTextFlags_AllowTabInput
        | ImGuiInputTextFlags_CallbackAlways
    ;
    bool p_open = true;

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
        flags,
        editor_callback
    );
    ImGui::SetItemDefaultFocus();

    if (written)
    {
        dirty = true;
    }

    if (ImGui::BeginPopupContextItem("##editor"))
    {
        if (ImGui::MenuItem("Generate word"))
        {
            auto word = word_chain.get_word_bigrams();
            auto pos = callback.CursorPos;
            string_insert(text + pos, word.c_str());
            dirty = true;
        }
        if (ImGui::BeginMenu("Generate multiple word options"))
        {
            if (ImGui::MenuItemPersistent("Regen"))
            {
                word_options.clear();
            }

            ImGui::Separator();

            if (word_options.size() == 0)
            {
                // Generate options.
                for (size_t i = 0; i < 10; ++i)
                {
                    auto word = word_chain.get_word_bigrams();
                    word_options.push_back(word);
                }
            }
            // Display options.
            for (size_t i = 0; i < 10; ++i)
            {
                if (ImGui::MenuItem(word_options[i].c_str()))
                {
                    auto pos = callback.CursorPos;
                    string_insert(text + pos, word_options[i].c_str());
                    word_options.clear();
                    dirty = true;
                }
            }

            ImGui::EndMenu();
        }
        if (ImGui::MenuItem("Generate line"))
        {
            auto line = word_chain.get_line_bigrams();
            auto pos = callback.CursorPos;
            string_insert(text + pos, line.c_str());
            dirty = true;
        }
        if (ImGui::BeginMenu("Generate multiple line options"))
        {
            if (ImGui::MenuItemPersistent("Regen"))
            {
                options.clear();
            }

            ImGui::Separator();

            if (options.size() == 0)
            {
                // Generate options.
                for (size_t i = 0; i < 5; ++i)
                {
                    auto line = word_chain.get_line_bigrams();
                    options.push_back(line);
                }
            }
            // Display options.
            for (size_t i = 0; i < 5; ++i)
            {
                if (ImGui::MenuItem(options[i].c_str()))
                {
                    auto pos = callback.CursorPos;
                    string_insert(text + pos, options[i].c_str());
                    dirty = true;
                    options.clear();
                }
            }

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }
    else
    {
        // Popup just closed.
        options.clear();
    }

    // Finish window.
    ImGui::End();
}

}
