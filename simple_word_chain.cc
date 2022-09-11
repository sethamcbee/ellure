/**
 * @file simple_word_chain.cc
 */

#include "simple_word_chain.h"

namespace Ellure
{

SimpleWordChain::SimpleWordChain(const std::vector<std::string>& input)
{
    for (size_t i = 0; i < input.size() - 1; ++i)
    {
        data.insert(input[i], input[i + 1]);
    }
    data.prepare();
    data.reset();
}

std::string SimpleWordChain::get_line()
{
    std::string output;
    std::string last;
    do
    {
        const auto& t = data.get();

        if (last != "" && last != "\n" && t != ",")
        {
            output += " ";
        }

        if (t != "\n")
        {
            output += t;
        }
        else
        {
            data.reset();
        }

        // Alpha.
        if (!(rand() % 10))
        {
            data.reset();
        }

        last = t;
    }
    while (last != "\n");

    return output;
}

size_t SimpleWordChain::size_bytes() const
{
    size_t size = sizeof(SimpleWordChain);
    size += data.size_bytes();
    return size;
}

}
