/**
 * @file editor.h
 */

#pragma once

#include <string>
#include <vector>

#include "complex_word_chain.h"

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
    size_t cursor_pos = 0;
    size_t selection_start = 0;
    size_t selection_end = 0;

    // GUI state.
    std::vector<std::string> word_options;
    std::vector<std::string> options;
};

}
