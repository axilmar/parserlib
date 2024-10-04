#include <iostream>
#include "parserlib.hpp"


extern void parserlib_core_unitTests();


static void unitTests() {
    parserlib_core_unitTests();
}


int main() {
    unitTests();
    system("pause");
    return 0;
}
