// Tests.cpp : This file contains the 'main' function. Program execution begins
// and ends there.
//

#include <iostream>

#define CATCH_CONFIG_MAIN
#include "thirdparty_catch_amalgamated.hpp"

int
main(int argc, char* argv[])
{
    // use tags for particular cases - for example, add [Base::mat3_t] in Command
    // Arguments in Debugging section run all tests by default if Tests project is
    // selected
    int result = Catch::Session().run(argc, argv);
    return result;
}
