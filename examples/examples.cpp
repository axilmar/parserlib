#include <iostream>
#include "json.hpp"
#include "xml.hpp"


using namespace parserlib;


static void run_example_json() {
    std::string src = R"({
        "v1": null,
        "v2": false,
        "v3": true,
        "v4": [false, true, {"x":5, "y":6}, 15, "aaa", null],
        "v5": { "a" : 32 },
        "v6": 17,
        "v7": "abc"
    })";

    const auto result = json::parse(src);

    std::cout << "JSON parser success = " << result->success << std::endl;

    if (!result->ast.empty()) {
        std::cout << "\nJSON parsed:\n";

        result->ast[0]->visit([&](const auto& node, size_t depth) {
            std::cout << std::string(depth * 4, ' ') << get_id_name(node.id());

            switch (node.id()) {
            case json::AST_ID::STRING:
                std::cout << ": " << node.begin()->start_position().to_string() << ": " << node.content();
                break;

            case json::AST_ID::NUMBER:
                std::cout << ": " << node.begin()->start_position().to_string() << ": \"" << node.content() << '"';
                break;

            default:
                break;
            }

            std::cout << std::endl;
        });
    }

    if (!result->errors.empty()) {
        std::cout << "\nJSON parse errors: " << std::endl;
        for (const auto& error : result->errors) {
            std::cout << "    ERROR: " << error.start_position().to_string() << ": " << get_id_name(error.id()) << std::endl;
        }
    }

    std::cout << "\n\n";
}


static void run_example_xml() {
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


void run_examples() {
    std::cout << std::boolalpha;
    run_example_json();
    run_example_xml();
}
