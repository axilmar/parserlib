#include <sstream>
#include <cassert>
#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


//the parser
using p = parser<>;


//the match ids
enum MATCH_ID {
    IDENTIFIER,
    STRING,
    ATTRIBUTE,
    ATTRIBUTE_LIST,
    OPENING_TAG,
    CLOSING_TAG,
    SELF_CLOSING_TAG,
    TEXT_NODE,
    NODE
};


extern p::rule node;


static auto ws = *p::range('\0', ' ');


static auto digit
    = p::range('0', '9')
    ;


static auto letter
    = p::range('a', 'z')
    | p::range('A', 'Z')
    ;


static auto identifier
    = (letter >> *(letter | digit)) ->* IDENTIFIER
    ;


static auto string
    = ('"' >> *(p::any() - '"') >> '"') ->* STRING
    ;


static auto attribute
    = (ws >> identifier >> ws >> '=' >> ws >> string) ->* ATTRIBUTE
    ;


static auto attribute_list
    = (*attribute) ->* ATTRIBUTE_LIST
    ;


static auto special_char
    = p::term("&lt;")
    | "&gt;"
    | "&amp;"
    | "&apos;"
    | "&quot;"
    ;


static auto text_char
    = special_char
    | p::any() - p::set("<>&'\"")
    ;


static auto text_node
    = (+text_char) ->* TEXT_NODE
    ;


static auto content_node
    = node
    | text_node
    ;


static auto closing_tag
    = (ws >> "</" >> identifier >> ws >> '>') ->* CLOSING_TAG
    ;


static auto node_head =
    ws >> p::begin_match() >> '<' >> identifier >> attribute_list
    ;


static auto node_tail
    = ws >> '>' >> p::add_match(OPENING_TAG) >> p::end_match() >> *content_node >> closing_tag
    | ws >> "/>" >> p::add_match(SELF_CLOSING_TAG) >> p::end_match()
    ;


static p::rule node
    = (node_head >> node_tail) ->* NODE
    ;


static auto grammar
    = node >> ws >> p::end()
    ;


template <class T>
static void render_space(T& stream, const char* space, size_t depth, size_t tab_size = 4) {
    for (size_t i = 0; i < depth * tab_size; ++i) {
        stream << space;
    }
}


template <class Stream>
static void render_xml(Stream& stream, const p::match_instance& match, const char* space = "", const char* nl = "", size_t depth = 0) {
    switch (match.get_id()) {
        case IDENTIFIER:
            stream << match.get_source();
            break;

        case STRING:
            stream << match.get_source();
            break;

        case ATTRIBUTE:
            stream << " ";
            render_xml(stream, match.get_children()[0], space, nl);
            stream << '=';
            render_xml(stream, match.get_children()[1], space, nl);
            break;

        case ATTRIBUTE_LIST:
            for (const auto& child : match.get_children()) {
                render_xml(stream, child, space, nl);
            }
            break;

        case OPENING_TAG:
            stream << '<';
            for (const auto& child : match.get_children()) {
                render_xml(stream, child, space, nl);
            }
            stream << ">" << nl;
            break;

        case CLOSING_TAG:
            stream << "</";
            render_xml(stream, match.get_children()[0], space, nl);
            stream << ">" << nl;
            break;

        case SELF_CLOSING_TAG:
            stream << '<';
            for (const auto& child : match.get_children()) {
                render_xml(stream, child, space, nl);
            }
            stream << "/>" << nl;
            break;

        case TEXT_NODE:
            render_space(stream, space, depth);
            stream << match.get_source() << nl;
            break;

        case NODE:
            //render the opening tag
            render_space(stream, space, depth);
            render_xml(stream, match.get_children()[0], space, nl);

            //if it is not self closing, render the content and the closing tag
            if (match.get_children()[0].get_id() == OPENING_TAG) {
                //render children
                for (size_t i = 1; i < match.get_children().size() - 1; ++i) {
                    render_xml(stream, match.get_children()[i], space, nl, depth + 1);
                }

                //render closing tag
                render_space(stream, space, depth);
                render_xml(stream, match.get_children()[match.get_children().size() - 1], space, nl);
            }
            break;
    }
}


static auto parse_xml(const std::string& input) {
    p::parse_context pc{ input };
    const bool ok = grammar.parse(pc);
    assert(ok);
    assert(pc.get_matches().size() == 1);
    std::stringstream stream;
    render_xml(stream, pc.get_matches()[0]);
    const std::string reconstructed_input = stream.str();
    assert(reconstructed_input == input);
    render_xml(std::cout, pc.get_matches()[0], " ", "\n");
}


void run_example_xml() {
    std::cout << "Parserlib example: xml\n";

    parse_xml("<root><child1 attr1=\"value1\"></child1><child2 attr2=\"value2\" attr3=\"value3\"/></root>");

    std::cout << "\n";
}
