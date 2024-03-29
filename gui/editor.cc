/**
 * @file editor.cc
 */

#include "gui/editor.h"

#include "doc.h"
#include "imgui_custom.h"
#include "gui/gui.h"
#include "pos.h"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

extern std::string lapos_main(const std::string& input);
extern void tokenize_ellure(const std::string & s1, std::vector<std::string> & lt);

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

LineState::LineState()
{
    tokens.insert(tokens.begin(), "[START]");
    tokens.push_back("[END]");
    pos.insert(pos.begin(), "[START]");
    pos.push_back("[END]");
}

LineState::LineState(const char* start, const char* end)
{
    line = "";
    auto p = start;
    while (p != end)
    {
        line += *p;
        ++p;
    }

    tokens.clear();
    tokenize_ellure(line, tokens);
    tokens.insert(tokens.begin(), "[START]");
    tokens.push_back("[END]");

    auto pos_str = lapos_main(line);
    POS pos_data{pos_str};
    pos = pos_data.get_data();
}

Editor::Editor()
{
    strcpy(text, "");

    // Open config file.
    const char* config_filename = "data/config.ini";
    std::ifstream config_file(config_filename);
    // TODO: Check if config file opened.

    // Read config file.
    std::string line;
    while (std::getline(config_file, line))
    {
        if (line == "")
        {
            // Ignore blank lines.
            continue;
        }
        active_banks.push_back(line);
    }

    // Read each file in each bank.
    for (const auto& bank : active_banks)
    {
        auto bank_path = "data/banks/" + bank;
        fs::recursive_directory_iterator dir_it{bank_path};
        for (const auto& dir_entry : dir_it)
        {
            if (!dir_entry.is_regular_file())
            {
                // Skip over subdirectories.
                continue;
            }
            std::string file_path{dir_entry.path().string()};
            Doc doc{file_path};
            ComplexWordChain chain{doc.get_words()};
            //chain.scale(1.0 / chain.get_size());
            word_chain.merge(chain);
        }
    }
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
    ImGui::Begin("Editor##editor", &p_open,
        ImGuiWindowFlags_NoDecoration
        | ImGuiWindowFlags_NoResize);

    auto editor_height = ImGui::GetWindowHeight() - 4.0f * ImGui::GetTextLineHeight();
    bool written = ImGui::InputTextMultiline(
        "##text",
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
        update_context();
    }

    if (ImGui::BeginPopupContextItem("##editorcontext"))
    {
        if (ImGui::MenuItem("Generate word"))
        {
            //auto word = word_chain.get_word_bigrams();
            auto word = word_chain.get_word_state(current_line);
            auto pos = callback.CursorPos;
            string_insert(text + pos, word.c_str());
            dirty = true;
        }
        if (ImGui::BeginMenu("Generate multiple word options##multword"))
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
                    //auto word = word_chain.get_word_bigrams();
                    auto word = word_chain.get_word_state(current_line);
                    word_options.push_back(word);
                }
            }
            // Display options.
            for (size_t i = 0; i < word_options.size(); ++i)
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
        if (ImGui::MenuItem("Generate line##"))
        {
            //auto line = word_chain.get_line_bigrams();
            auto line = word_chain.get_line_state(current_line);
            auto pos = callback.CursorPos;
            string_insert(text + pos, line.c_str());
            dirty = true;
        }
        if (ImGui::BeginMenu("Generate multiple line options##multline"))
        {
            if (ImGui::MenuItemPersistent("Regen##regen"))
            {
                options.clear();
            }

            ImGui::Separator();

            if (options.size() == 0)
            {
                // Generate options.
                for (size_t i = 0; i < 5; ++i)
                {
                    //auto line = word_chain.get_line_bigrams();
                    auto line = word_chain.get_line_state(current_line);
                    options.push_back(line);
                }
            }
            // Display options.
            for (size_t i = 0; i < options.size(); ++i)
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

void Editor::update_context()
{
    // Find start of current line.
    size_t cursor = (size_t)callback.CursorPos;
    size_t line_start = cursor;
    while (line_start > 0 && text[line_start] != '\n')
    {
        --line_start;
    }

    current_line = LineState{text + line_start, text + cursor};
}

}
