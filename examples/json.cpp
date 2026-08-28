#include <sstream>
#include <cassert>
#include <iostream>
#include "parserlib.hpp"


//grammar found at: https://www.json.org/json-en.html


// TOKENIZER ==================================================================


enum class TOKEN_ID {
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    NULL_,
    LEFT_CURLY_BRACKET,
    RIGHT_CURLY_BRACKET,
    LEFT_SQUARE_BRACKET,
    RIGHT_SQUARE_BRACKET,
    COMMA,
    COLON
};


enum class TOKEN_ERROR_ID {
    SYNTAX_ERROR
};


using t = parserlib::parser<std::string::const_iterator, TOKEN_ID, TOKEN_ERROR_ID>;


static auto ws
    = t::term(' ') 
    | '\x0A' 
    | '\x0D' 
    | '\x09'
    ;


static auto sign
    = -(t::term('+') | '-')
    ;


static auto exponent
    = -(t::term('e') | 'E')
    ;


static auto digit
    = t::range('0', '9')
    ;


static auto one_nine
    = t::range('1', '9')
    ;


static auto digits
    = +digit
    ;


static auto fraction
    = -('.' >> digits)
    ;


static auto integer
    = digit
    | one_nine >> digits
    | '-' >> digit
    | '-' >> one_nine >> digits
    ;


static auto number
    = integer >> fraction >> exponent
    ;


static auto hex
    = digit
    | t::range('a', 'f')
    | t::range('A', 'F')
    ;


static auto escape
    = t::term('"')
    | '\\'
    | '/'
    | 'b'
    | 'f'
    | 'n'
    | 'r'
    | 't'
    | 'u' >> hex >> hex >> hex >> hex
    ;


static auto character
    = t::range(0x0020, 0x10FFFF) - '"' - '\\'
    ;


static auto characters
    = *character
    ;


static auto string
    = '"' >> characters >> '"'
    ;


static auto token
    = string ->* TOKEN_ID::STRING
    | number ->* TOKEN_ID::NUMBER
    | t::term("true") ->* TOKEN_ID::TRUE
    | t::term("false") ->* TOKEN_ID::FALSE
    | t::term("null") ->* TOKEN_ID::NULL_
    | t::term('{') ->* TOKEN_ID::LEFT_CURLY_BRACKET
    | t::term('}') ->* TOKEN_ID::RIGHT_CURLY_BRACKET
    | t::term('[') ->* TOKEN_ID::LEFT_SQUARE_BRACKET
    | t::term(']') ->* TOKEN_ID::RIGHT_SQUARE_BRACKET
    | t::term(',') ->* TOKEN_ID::COMMA
    | t::term(':') ->* TOKEN_ID::COLON
    ;


static auto valid_token_element
    = ws
    | token
    ;


static auto token_element
    = valid_token_element
    | t::error(t::skip_before(valid_token_element | t::end()), TOKEN_ERROR_ID::SYNTAX_ERROR)
    ;


static auto tokenizer
    = *token_element >> t::end()
    ;


// PARSER =====================================================================


enum class PARSER_ID {
    OBJECT,
    ARRAY,
    MEMBER,
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    NULL_,
    NAME
};


using parser_input = std::vector<t::match_instance>;


using p = parserlib::parser<parser_input::const_iterator, PARSER_ID>;


extern p::rule element;


static auto member_name
    = p::term(TOKEN_ID::STRING) ->* PARSER_ID::NAME
    ;


static auto member
    = (member_name >> TOKEN_ID::COLON >> element) ->* PARSER_ID::MEMBER
    ;


static auto members 
    = member >> *(TOKEN_ID::COMMA >> member)
    ;


static auto object
    = (TOKEN_ID::LEFT_CURLY_BRACKET >> -members >> TOKEN_ID::RIGHT_CURLY_BRACKET) ->* PARSER_ID::OBJECT
    ;


static auto elements
    = element >> *(TOKEN_ID::COMMA >> element)
    ;


static auto array
    = (TOKEN_ID::LEFT_SQUARE_BRACKET >> -elements >> TOKEN_ID::RIGHT_SQUARE_BRACKET) ->* PARSER_ID::ARRAY
    ;


static auto value
    = object
    | array
    | p::term(TOKEN_ID::STRING) ->* PARSER_ID::STRING
    | p::term(TOKEN_ID::NUMBER) ->* PARSER_ID::NUMBER
    | p::term(TOKEN_ID::TRUE) ->* PARSER_ID::TRUE
    | p::term(TOKEN_ID::FALSE) ->* PARSER_ID::FALSE
    | p::term(TOKEN_ID::NULL_) ->* PARSER_ID::NULL_
    ;


static p::rule element
    = value 
    ;


static auto parser = element;


template <class T>
static void render_space(T& stream, const char* space, size_t depth, size_t tab_size = 4) {
    for (size_t i = 0; i < depth * tab_size; ++i) {
        stream << space;
    }
}


template <class Stream>
static void render_json(Stream& stream, const typename p::match_instance& match, const char* space = "", const char* nl = "", size_t depth = 0) {
    switch (match.get_id()) {
        case PARSER_ID::OBJECT:
            stream << '{' << nl;
            for (size_t i = 0; i < match.get_children().size(); ++i) {
                const auto& child = match.get_children()[i];
                render_space(stream, space, depth + 1);
                render_json(stream, child, space, nl, depth + 1);
                if (i < match.get_children().size() - 1) {
                    stream << ',';
                }
                stream << nl;
            }
            render_space(stream, space, depth);
            stream << '}';
            break;

        case PARSER_ID::ARRAY:
            stream << '[' << nl;
            for (size_t i = 0; i < match.get_children().size(); ++i) {
                const auto& child = match.get_children()[i];
                render_space(stream, space, depth + 1);
                render_json(stream, child, space, nl, depth + 1);
                if (i < match.get_children().size() - 1) {
                    stream << ',';
                }
                stream << nl;
            }
            render_space(stream, space, depth);
            stream << ']';
            break;

        case PARSER_ID::MEMBER:
            render_json(stream, match.get_children()[0], space, nl, depth);
            stream << ": ";
            render_json(stream, match.get_children()[1], space, nl, depth);
            break;

        case PARSER_ID::STRING:
            stream << match.get_origin_source();
            break;

        case PARSER_ID::NUMBER:
            stream << match.get_origin_source();
            break;

        case PARSER_ID::TRUE:
            stream << "true";
            break;

        case PARSER_ID::FALSE:
            stream << "false";
            break;

        case PARSER_ID::NULL_:
            stream << "null";
            break;

        case PARSER_ID::NAME:
            stream << match.get_origin_source();
            break;
    }
}


static void parse_json(const std::string& input) {
    //tokenize
    t::parse_context tokenizer_pc{ input };
    const bool tokenizer_ok = tokenizer.parse(tokenizer_pc);
    assert(tokenizer_ok);

    //parse
    p::parse_context parser_pc{ tokenizer_pc.get_matches() };
    const bool parser_ok = parser.parse(parser_pc);
    assert(parser_ok);

    //check
    std::stringstream stream;
    render_json(stream, parser_pc.get_matches()[0]);
    const std::string reconstructed_input = stream.str();
    assert(reconstructed_input == input);

    render_json(std::cout, parser_pc.get_matches()[0], " ", "\n");
}


void run_example_json() {
    parse_json("{\"x\": 0,\"y\": 1,\"data\": [true,false,null,{\"a\": \"abc\"}]}");
}
