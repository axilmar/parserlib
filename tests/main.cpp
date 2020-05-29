#include <iostream>


extern void runCalculatorExampleTests();
extern void runCalculatorDemo();
extern void runCalculatorExample2Tests();
extern void runCalculator2Demo();
extern void runXMLExampleTests();


int main()
{
    #ifdef DEMO
    runCalculatorExampleTests();
    runCalculatorExample2Tests();
    runXMLExampleTests();
    #ifndef NDEBUG
    system("pause");
    #endif
    #else
    //runCalculatorDemo();
    runCalculator2Demo();
    #endif
    return 0;
}
