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
#include "gui/gui.h"
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

    int gui_ret = Ellure::gui_main();
    return gui_ret;

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
    std::cout << "=====\nunigrams:\n=====\n";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << word_chain.get_line_unigrams() << std::endl;
    }
    std::cout << "=====\nbigrams:\n=====\n";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << word_chain.get_line_bigrams() << std::endl;
    }
    std::cout << "=====\ntrigrams:\n=====\n";
    for (size_t i = 0; i < 10; ++i)
    {
        std::cout << word_chain.get_line_trigrams() << std::endl;
    }
    
    return 0;
}
