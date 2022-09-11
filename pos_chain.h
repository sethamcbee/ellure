/**
 * @file pos_chain.h
 */

#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "chain.h"

namespace Ellure
{

class POSChain
{
public:

    POSChain(const std::vector<std::string>& input);

    Weight calc_line_weight(const std::string& input);

private:

    Chain<std::tuple<std::string, std::string, std::string>> data;
};
    
}
