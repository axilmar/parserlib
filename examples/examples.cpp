#include <iostream>
#include "json.hpp"


using namespace parserlib;


static void run_example_json() {
    std::string src = 
R"({
    "v1": null,
    "v2": false,
    "v3": true,
    "v4": [false, true, [1, 2, 3], {"x":5, "y":6}, 15, "aaa", null],
    "v5": { "a" : 32 },
    "v6": 17,
    "v7": "abc"
}
)";

    using json_parser = json<>;

    const auto result = json_parser::parse(src);

    std::cout << "JSON parser success = " << result->success << std::endl;

    if (!result->ast.empty()) {
        std::cout << "\nJSON parsed:\n";

        result->ast[0]->visit([&](const auto& node, size_t depth) {
            std::cout << std::string(depth * 4, ' ') << json<>::ast_id_name(node.id());

            switch (node.id()) {
            case json_parser::AST_ID::STRING:
                std::cout << ": " << node.begin()->start_position().to_string() << ": " << node.content();
                break;

            case json_parser::AST_ID::NUMBER:
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
            std::cout << "    ERROR: " << error.start_position().to_string() << ": " << json_parser::error_id_name(error.id()) << std::endl;
        }
    }
}


void run_examples() {
    std::cout << std::boolalpha;
    run_example_json();
}
