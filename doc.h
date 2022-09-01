/**
 * @file doc.h
 */

#pragma once

#include <string>
#include <vector>

namespace Ellure
{

class Doc
{
public:

    Doc(const std::string& filename);

    const std::vector<std::string>& get_words() const;

private:

    std::vector<std::string> words;

    const char* get_word(const char* input);
};

}
