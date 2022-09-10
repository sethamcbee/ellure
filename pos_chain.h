/**
 * @file pos_chain.h
 */

#pragma once

#include <string>
#include <vector>

#include "chain.h"

namespace Ellure
{

class POSChain
{
public:

    POSChain(const std::vector<std::string>& input);

private:

    Chain<std::string> data;
};
    
}
