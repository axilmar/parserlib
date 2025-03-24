#include "parserlib.hpp"


using namespace parserlib;


enum class source_token_id {
    A,
    B,
    C
};


struct token {
    source_token_id id;
    std::string source;
    std::size_t line;
    std::size_t column;
};


static int operator - (const token& a, const source_token_id& b) {
    return static_cast<int>(a.id) - static_cast<int>(b);
}


enum class ast_token_id {
    A,
    B,
    C
};


void test_custom_token_parsing() {
    const auto grammar = +(
        (terminal(source_token_id::A) ->* ast_token_id::A) | 
        (terminal(source_token_id::B) ->* ast_token_id::B) |
        (terminal(source_token_id::C) ->* ast_token_id::C)
    );

    using ParseDefinitions = parse_definitions<std::vector<token>, ast_token_id>;
    using ParseContext = parse_context<ParseDefinitions>;

    std::vector<token> source;
    source.push_back(token{ source_token_id::A, "a", 1, 1 });
    source.push_back(token{ source_token_id::B, "b", 1, 1 });
    source.push_back(token{ source_token_id::C, "c", 1, 1 });

    ParseContext context(source);
    assert(grammar.parse(context));

    assert(context.matches().size() == 3);
    assert(context.matches()[0].id() == ast_token_id::A);
    assert(context.matches()[1].id() == ast_token_id::B);
    assert(context.matches()[2].id() == ast_token_id::C);
}
