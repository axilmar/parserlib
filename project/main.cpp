#include <iostream>
#include "parserlib.hpp"


extern void parserlib_core_unitTests();
extern void parserlib_cfe_unitTests();


static void unitTests() {
    parserlib_core_unitTests();
    parserlib_cfe_unitTests();
}


int main() {
    unitTests();
    system("pause");
    return 0;
}
