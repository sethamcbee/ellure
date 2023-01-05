/**
 * @file editor.h
 */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "complex_word_chain.h"
#include "imgui_custom.h"

class ImGuiInputTextCallbackData;

namespace Ellure
{

struct LineState
{
    LineState();
    LineState(const char* start, const char* end);

    std::vector<std::string> tokens;
    std::vector<std::string> pos;
};

class Editor
{
public:

    Editor();

    void run();

    // Get context of current line.
    void update_context();

    // Config data.
    std::vector<std::string> active_banks;

    // Runtime data.
    char text[1024 * 160];
    ComplexWordChain word_chain;
    bool dirty = false;
    LineState current_line;

    // Callback state.
    ImGuiInputTextCallbackData callback;

    // GUI state.
    std::vector<std::string> word_options;
    std::vector<std::string> options;
};

}
