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

    // GUI state.
    std::vector<std::string> options;
};

}
