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

    size_t size_bytes() const;

private:

    Chain<std::string> data;
};

}
