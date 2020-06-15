#include <iostream>


extern void run_calculator_tests();
extern void run_xml_tests();


int main(int argc, char* argv[])
{
    run_calculator_tests();
    run_xml_tests();
    system("pause");
    return 0;
}
