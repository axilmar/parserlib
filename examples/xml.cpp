#include <iostream>
#include "xml.hpp"


using namespace parserlib;


void run_example_xml() {
    std::string src = R"(
        <?xml version="1.0" encoding="UTF-8" ?>
        <root>
            content1
            <node1 attr1="attr1" >
            </node1>
            content2
            <node2 attr2="attr2" attr3 = "attr3" />
        </root>
    )";

    const auto result = xml::parse(src);

    std::cout << "xml result parsing = " << std::get<0>(result) << std::endl;

    std::cout << "xml tree:\n";
    for (const auto& node : std::get<2>(result)) {
        node->visit([](const auto& node, size_t depth) {
            switch (node.id()) {
                case xml::ID::PROLOG:
                    std::cout << std::string(depth*4, ' ') << "prolog" << std::endl;
                    break;

                case xml::ID::ATTRIBUTE_LIST:
                    std::cout << std::string(depth*4, ' ') << "attribute list" << std::endl;
                    break;

                case xml::ID::ATTRIBUTE:
                    std::cout << std::string(depth*4, ' ') << "attribute" << std::endl;
                    break;

                case xml::ID::ATTRIBUTE_NAME:
                    std::cout << std::string(depth*4, ' ') << "name:" << node.content() << std::endl;
                    break;

                case xml::ID::ATTRIBUTE_VALUE:
                    std::cout << std::string(depth*4, ' ') << "value:" << node.content() << std::endl;
                    break;

                case xml::ID::ELEMENT_NODE:
                    std::cout << std::string(depth*4, ' ') << "element" << std::endl;
                    break;

                case xml::ID::ELEMENT_OPENING_TAG:
                    std::cout << std::string(depth*4, ' ') << "opening tag: " << node.content() << std::endl;
                    break;

                case xml::ID::ELEMENT_CONTENT:
                    std::cout << std::string(depth*4, ' ') << "content:" << std::endl;
                    break;

                case xml::ID::ELEMENT_CLOSING_TAG:
                    std::cout << std::string(depth*4, ' ') << "closing tag: " << node.content() << std::endl;
                    break;

                case xml::ID::TEXT_NODE:
                    std::cout << std::string(depth*4, ' ') << "text: " << node.content() << std::endl;
                    break;
            }
        });
    }

    std::cout << "\n\n";
}
