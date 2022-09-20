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

    // Initialization.
    srand(time(NULL));
    Ellure::User user{argv[1]};
    Ellure::Doc doc{"data/testdoc"};
    lapos_init();

    int gui_ret = Ellure::gui_main();
    return gui_ret;
}
