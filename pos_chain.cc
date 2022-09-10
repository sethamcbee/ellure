/**
 * @file pos_chain.cc
 */

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
            Ellure::POS pos{line_pos};
            const auto& pos_data = pos.get_data();

            // Catch blank lines.
            if (pos_data[0].size() == 0)
            {
                line = "";
                continue;
            }

            // Catch lines with a single word.
            if (pos_data[0].size() == 1)
            {
                data.insert(pos_data[0][0], "END");
                line = "";
                continue;
            }

            // Else, add to chain.
            for (size_t j = 0; j < pos_data[0].size() - 1; ++j)
            {
                data.insert(pos_data[0][j], pos_data[0][j + 1]);
            }
            data.insert(pos_data[0][pos_data[0].size() - 1], "END");
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

}
