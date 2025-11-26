#include <iostream>


extern void run_example_json();
extern void run_example_xml();


void run_examples() {
    std::cout << std::boolalpha;
    run_example_json();
    run_example_xml();
}
