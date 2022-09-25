/**
 * @file editor.h
 */

#pragma once

#include <string>
#include <vector>

#include "complex_word_chain.h"
#include "imgui_custom.h"

class ImGuiInputTextCallbackData;

namespace Ellure
{

class Editor
{
public:

    Editor();

    void run();

    // Runtime data.
    char text[1024 * 160];
    ComplexWordChain word_chain;
    bool dirty = false;

    // Callback state.
    ImGuiInputTextCallbackData callback;

    // GUI state.
    std::vector<std::string> word_options;
    std::vector<std::string> options;
};

}
