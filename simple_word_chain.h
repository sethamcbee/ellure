/**
 * @file simple_word_chain.h
 */

#pragma once

#include <string>
#include <vector>

#include "chain.h"

namespace Ellure
{

class SimpleWordChain
{
public:

    SimpleWordChain(const std::vector<std::string>& input);

    std::string get_line();

private:

    Chain<std::string> data;
};

}
