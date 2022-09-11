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
    std::string line;
    for (size_t i = 0; i < input.size() - 1; ++i)
    {
        if (input[i] == "\n")
        {
            auto line_pos = lapos_main(line);
            POS pos{line_pos};
            const auto& pos_data = pos.get_data()[0];

            // Catch blank lines.
            if (pos_data.size() == 0)
            {
                line = "";
                continue;
            }

            // Catch lines with a single word.
            if (pos_data.size() == 1)
            {
                auto gram0 = std::tuple{pos_data[0], "END"};
                auto gram1 = std::tuple{"END", "END"};
                data.insert(gram0, gram1);
                line = "";
                continue;
            }

            // Else, add to chain normally.
            for (size_t j = 0; j < pos_data.size() - 2; ++j)
            {
                auto gram0 = std::tuple{pos_data[j], pos_data[j + 1]};
                auto gram1 = std::tuple{pos_data[j + 1], pos_data[j + 2]};
                data.insert(gram0, gram1);
            }
            auto gram0 = std::tuple{pos_data[pos_data.size() - 1], "END"};
            auto gram1 = std::tuple{"END", "END"};
            data.insert(gram0, gram1);
            line = "";
        }
        else
        {
            if (line != "" && input[i] != "," && input[i] != "!" && input[i] != "?")
            {
                line += " ";
            }
            line += input[i];
        }
    }

    data.prepare();
    data.reset();
}

Weight POSChain::calc_line_weight(const std::string& input)
{
    auto line_pos = lapos_main(input);
    POS pos{line_pos};
    const auto& pos_data = pos.get_data()[0];

    // Catch blank lines.
    if (pos_data.size() == 0)
    {
        return 0;
    }

    // Catch lines with a single word.
    if (pos_data.size() == 1)
    {
        auto gram0 = std::tuple{pos_data[0], "END"};
        auto gram1 = std::tuple{"END", "END"};
        return data.get_weight(gram0, gram1);
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
    return log(total) / pos_data.size();
}

}
