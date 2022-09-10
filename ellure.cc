/**
 * @file ellure.cc
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "chain.h"
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
    Ellure::SimpleWordChain word_chain{strings};

    // Build sample output from Markov chain.
    auto output = word_chain.get_line();
    std::cout << output << std::endl;

    // POS tag the sample output.
    auto pos_output = lapos_main(output);
    std::cout << "=====\n" << pos_output;
    Ellure::POS pos{pos_output};
    
    return 0;
}
