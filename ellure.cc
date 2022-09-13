/**
 * @file ellure.cc
 */

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "chain.h"
#include "complex_word_chain.h"
#include "doc.h"
#include "pos.h"
#include "pos_chain.h"
#include "simple_word_chain.h"
#include "user.h"
#include "weighted_vector.h"

extern void lapos_init();
extern std::string lapos_main(const std::string& input);

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Invalid arguments.\n";
        return 1;
    }

    // STUB initialization.
    srand(time(NULL));
    Ellure::User user{argv[1]};
    Ellure::Doc doc{"data/testdoc"};
    lapos_init();

    // Build POS order likelihoods from test data.
    const auto& strings = doc.get_words();
    Ellure::POSChain pos_chain{strings};

    // Build Markov chain from test data.
    Ellure::ComplexWordChain word_chain{strings};

    // Test.
    std::vector<std::tuple<Ellure::Weight, std::string>> lines;
    for (size_t i = 0; i < 100; ++i)
    {
        auto line = word_chain.get_line();
        auto weight = pos_chain.calc_line_weight(line);
        auto tmp = std::tuple{weight, line};
        lines.push_back(tmp);
    }
    std::sort(lines.begin(), lines.end());
    for (const auto& pair : lines)
    {
        std::cout << std::get<1>(pair);
        std::cout << "\n: " << std::get<0>(pair) << std::endl;
    }
    
    return 0;
}
