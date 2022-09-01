/**
 * @file ellure.cc
 */

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "chain.h"
#include "doc.h"
#include "user.h"
#include "weighted_vector.h"

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

    // TEST.
    srand(time(NULL));
    Ellure::Chain<std::string> chain;
    const auto& strings = doc.get_words();
    for (size_t i = 0; i < strings.size() - 1; ++i)
    {
        chain.insert(strings[i], strings[i + 1]);
    }
    chain.prepare();
    chain.reset();

    std::string last;
    for (size_t i = 0; i < 150; ++i)
    {
        const auto& t = chain.get();

        if (last != "" && last != "\n" && t != ",")
        {
            std::cout <<  " ";
        }
        std::cout << t;

        // Alpha.
        if (!(rand() % 10))
        {
            chain.reset();
        }

        last = t;
    }
    std::cout << std::endl;
    
    return 0;
}
