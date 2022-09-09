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

    // STUB.
    Ellure::User user{argv[1]};

    Ellure::Doc doc{"data/testdoc"};

    lapos_init();

    // Build Markov chain from test data.
    srand(time(NULL));
    Ellure::Chain<std::string> chain;
    const auto& strings = doc.get_words();
    for (size_t i = 0; i < strings.size() - 1; ++i)
    {
        chain.insert(strings[i], strings[i + 1]);
    }
    chain.prepare();
    chain.reset();

    // Build sample output from Markov chain.
    std::string output;
    std::string last;
    for (size_t i = 0; i < 15; ++i)
    {
        const auto& t = chain.get();

        if (last != "" && last != "\n" && t != ",")
        {
            output += " ";
        }
        output += t;

        // Alpha.
        if (!(rand() % 10))
        {
            chain.reset();
        }

        last = t;
    }
    output += '\n';
    std::cout << output;

    // POS tag the sample output.
    auto pos_output = lapos_main(output);
    std::cout << "=====\n" << pos_output;

    // Build Markov chain of POS orders.
    Ellure::POS pos{pos_output};
    
    return 0;
}
