/**
 * @file pos.h
 */

#pragma once

#include <string>
#include <vector>

namespace Ellure
{

class POS
{
public:

    POS(const std::string& input);

    const std::vector<std::vector<std::string>>& get_data();

private:

    std::vector<std::vector<std::string>> data;
};

}
