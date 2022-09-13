/**
 * @file pos_chain.cc
 */

#include <cmath>

#include "pos.h"
#include "pos_chain.h"

extern std::string lapos_main(const std::string& input);

namespace Ellure
{

POSChain::POSChain(const std::vector<std::string>& input)
{
    for (size_t start = 0; start < input.size() - 2; ++start)
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

        // Build bigrams from POS data.
        for (size_t i = 0; i < pos_data.size() - 2; ++i)
        {
            auto gram0 = std::tuple{pos_data[i], pos_data[i + 1]};
            auto gram1 = std::tuple{pos_data[i + 1], pos_data[i + 2]};
            data.insert(gram0, gram1);
        }

        // Iterate to next line.
        start = end;
    }

    data.prepare();
    data.reset();
}

Weight POSChain::calc_line_weight(const std::string& input)
{
    auto line_pos = lapos_main(input);
    POS pos{line_pos};
    const auto& pos_data = pos.get_data();

    // Catch blank lines.
    if (pos_data.size() == 2)
    {
        return 0;
    }

    Weight last = 1.0;
    Weight total = 0.0;
    for (size_t i = 0; i < pos_data.size() - 2; ++i)
    {
        auto gram0 = std::tuple{pos_data[i], pos_data[i + 1]};
        auto gram1 = std::tuple{pos_data[i + 1], pos_data[i + 2]};
        auto sub_weight = data.get_weight(gram0, gram1);
        total += sub_weight * last;
        last = sub_weight;
    }
    return total / pos_data.size();
}

}
