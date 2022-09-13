/**
 * @file complex_word_chain.cc
 */

#include "complex_word_chain.h"

#include "pos.h"

extern std::string lapos_main(const std::string& input);

namespace Ellure
{

ComplexWordChain::ComplexWordChain(const std::vector<std::string>& input)
{
    // Associate each word with the POS preceding it.
    for (size_t start = 0; start < input.size(); ++start)
    {
        auto end = start + 1;
        while (input[end] != "[END]")
        {
            ++end;
        }
        if (end == start + 1)
        {
            // Skip empty lines.
            ++start;
            continue;
        }

        // Build line for lapos.
        std::string line{input[start + 1]};
        for (size_t i = start + 2; i < end; ++i)
        {
            if (input[i] != ",")
            {
                line += " ";
            }
            line += input[i];
        }
        auto pos_str = lapos_main(line);
        POS pos{pos_str};
        const auto& pos_data = pos.get_data();

        // Build chain, including START and END tokens.
        for (size_t i = 1; i < pos_data.size() - 1; ++i)
        {
            auto gram0 = std::tuple{pos_data[i - 1], input[start + i]};
            auto gram1 = std::tuple{pos_data[i], input[start + i + 1]};
            data.insert(gram0, gram1);
        }

        // Iterate to next line.
        start = end;
    }

    data.prepare();
    data.reset();
}

std::string ComplexWordChain::get_line()
{
    std::string last;
    std::string output;
    do
    {
        const auto& pair = data.get();
        auto t = std::get<1>(pair);

        if (t == "[END]")
        {
            return output;
        }

        if (t == "[START]")
        {
            continue;
        }

        if (t == "'s"
            || t == "'m"
            || t == "'d"
            || t == "'ll"
            || t == "'re"
            || t == "'ve"
            || t == "n't")
        {
            output += t;
            last = t;
            continue;
        }
        if (last == "Gim" || last == "gim" || last == "Lem" || last == "lem")
        {
            if (t == "me")
            {
                output += t;
                last = t;
                continue;
            }
        }
        if (last == "Gon" || last == "gon" || last == "Wan" || last == "wan")
        {
            if (t == "na")
            {
                output += t;
                last = t;
                continue;
            }
        }
        
        if (t != "," && output != "")
        {
            output += " ";
        }
        output += t;

        // Alpha.
        if (!(rand() % 10))
        {
            data.reset();
        }

        last = t;
    }
    while (1);

    return output;
}

}
