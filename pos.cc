/**
 * @file pos.cc
 */

#include "pos.h"

namespace Ellure
{

POS::POS(const std::string& input)
{
    std::vector<std::string> blank_line;
    data.push_back(blank_line);
    std::vector<std::string>* line = &data[0];
    for (size_t i = 0; i < input.size(); ++i)
    {
        // Skip characters until POS tag or newline.
        while (input[i] != '/')
        {
            if (input[i] == '\n')
            {
                data.push_back(blank_line);
                ++line;
                continue;
            }
            ++i;
        }
        // Tag found. Iterate until whitespace.
        ++i;
        std::string word;
        while (i < input.size() && input[i] != ' ' && input[i] != '\n')
        {
            word += input[i];
            ++i;
        }
        line->push_back(word);
    }
}

const std::vector<std::vector<std::string>>& POS::get_data()
{
    return data;
}

}
