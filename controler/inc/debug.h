#ifndef DEBUG_H
#define DEBUG_H

#include <string>
#include <iostream>

using namespace std;

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x) std::cout << __PRETTY_FUNCTION__ << x << std::endl;
#else
#define DEBUG_PRINT
#endif

#endif // DEBUG_H
