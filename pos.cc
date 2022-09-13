/**
 * @file pos.cc
 */

#include "pos.h"

namespace Ellure
{

POS::POS(const std::string& input)
{
    data.push_back("[START]");
    for (size_t i = 1; i < input.size(); ++i)
    {
        // Skip characters until POS tag or newline.
        while (input[i] != '/')
        {
            if (input[i] == '\n')
            {
                data.push_back("[END]");
                return;
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
        data.push_back(word);
    }
    data.push_back("[END]");
}

const std::vector<std::string>& POS::get_data()
{
    return data;
}

}
