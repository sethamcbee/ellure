/**
 * @file complex_word_chain.h
 */

#pragma once

#include <string>
#include <vector>

#include "chain.h"

namespace Ellure
{

class ComplexWordChain
{
public:

    // string1 = Previous POS, string2 = current word
    using Value = std::tuple<std::string, std::string>;

    ComplexWordChain(const std::vector<std::string>& input);

    std::string get_line();

private:

    Chain<Value> data;
};

}
