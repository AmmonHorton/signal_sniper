#include <iostream>
#include "test_module.h"

int TestModule::add(int a, int b) {
    std::cout << "I did something\n";
    return a + b;
}

