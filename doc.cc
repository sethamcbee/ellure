/**
 * @file doc.cc
 */

#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "doc.h"
#include "weighted_vector.h"

namespace Ellure
{

Doc::Doc(const std::string& filename)
{
    // Read input file.
    std::stringstream input;
    std::ifstream input_file{filename};
    if (!input_file.is_open())
    {
        std::cerr << "Error opening document.\n";
        exit(EXIT_FAILURE);
    }
    input << input_file.rdbuf();

    std::string str = input.str();
    const char* p = str.c_str();
    while (*p)
    {
        p = get_word(p);
    }
}

const std::vector<std::string>& Doc::get_words() const
{
    return words;
}

const char* Doc::get_word(const char* input)
{
    std::string word;
    
    auto is_good = [](char c)
    {
        switch (c)
        {
        case '"':
        case ',':
        case '.':
        case '?':
        case '!':
        case ';':
        case ':':
        case '(':
        case ')':
        case ' ':
        case '\n':
        case '\0':
            return false;
        default:
            return true;
        }
    };

    // Iterate if this word starts on a bad character.
    while (!is_good(*input) && *input != '\0')
    {        
        if (*input == ',' || *input == '\n')
        {
            word += *input;
            words.push_back(word);
            ++input;
            return input;
        }
        ++input;
    }
    if (*input == '\0')
    {
        return input;
    }

    do
    {
        word += tolower(*input);
        ++input;
    }
    while (is_good(*input));
    words.push_back(word);
    return input;
}

}
