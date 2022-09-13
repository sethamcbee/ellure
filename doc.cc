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

extern void tokenize_ellure(const std::string & s1, std::vector<std::string> & lt);

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

    // Use tokenizer from lapos.
    std::vector<std::string> tok_v;
    tokenize_ellure(input.str(), tok_v);
    std::string tok_str;
    tok_str += tok_v[0];
    for (size_t i = 1; i < tok_v.size(); ++i)
    {
        // Handle newlines.
        if (tok_v[i] == "[NEWLINE]")
        {
            // Condense multiple newlines.
            if (tok_v[i - 1] != "[NEWLINE]")
            {
                tok_str += '\n';
            }
            continue;
        }
        
        if (tok_v[i] != "," && tok_v[i - 1] != "[NEWLINE]")
        {
            tok_str += ' ';
        }
        tok_str += tok_v[i];
    }

    words.push_back("[START]");
    std::string str = tok_str;
    const char* p = str.c_str();
    while (*p)
    {
        p = get_word(p);
    }

    // Remove hanging sentence start.
    if (words[words.size() - 1] == "[START]")
    {
        words.pop_back();
    }
    words.shrink_to_fit();
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
        if (*input == ',')
        {
            word += *input;
            words.push_back(word);
            ++input;
            return input;
        }
        else if (*input == '\n')
        {
            words.push_back("[END]");
            words.push_back("[START]");
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
        word += *input;
        ++input;
    }
    while (is_good(*input));
    words.push_back(word);
    return input;
}

}
