#include <iostream>
#include <sstream>
#include "examples/xml/xml_ast.hpp"


using namespace parserlib;
using namespace xml;


void run_xml_tests()
{
    std::cout << "Running xml test...";

    element_ptr xml1 = element(
        name("root"), 
        {
            attribute("attr1", "value1"),
            attribute("attr2", "value2"),
        },
        {
            element(
                name("child1"), 
                {
                    attribute("attr3", "value3")
                }, 
                {
                }),
            content("the quick brown fox jumps over the lazy dog"),
            element(name("child2"), 
                {
                }, 
                {
                    element(name("child22"), {}, {})
                })
        });

    std::stringstream stream;
    xml1->to_xml(stream);

    const std::string input = stream.str();
    auto pc = parse_context(input);
    bool ok = parse(xml_grammar, pc);

    if (ok)
    {
        element_ptr xml2 = create_ast(pc);
        ok = xml1->equals(*xml2);
    }

    if (ok)
    {
        std::cout << "OK\n";
    }
    else
    {
        std::cout << "NOT OK\n";
    }
}
