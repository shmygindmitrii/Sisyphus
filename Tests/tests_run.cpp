// Tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#define CATCH_CONFIG_MAIN
#include "thirdparty_catch_amalgamated.hpp"

int main(int argc, char* argv[]) {
    int result = Catch::Session().run(argc, argv);
    return result;
}
