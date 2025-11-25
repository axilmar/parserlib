#ifndef PARSERLIB_XML_HPP
#define PARSERLIB_XML_HPP


#include <utility>
#include "parserlib.hpp"


namespace parserlib {


    class xml {
    public:
        enum class ID {
            PROLOG,
            ATTRIBUTE,
            ATTRIBUTE_NAME,
            ATTRIBUTE_VALUE,
            ATTRIBUTE_LIST,
            ELEMENT_NODE,
            ELEMENT_OPENING_TAG,
            ELEMENT_CONTENT,
            ELEMENT_CLOSING_TAG,
            TEXT_NODE
        };

        enum class ERROR_ID {
            INVALID_COMMENT_CHARACTERS
        };

        template <class Source> class parser {
        public:
            using parse_context_type = parse_context<Source, ID, ERROR_ID, text_position>;

            using rule_type = rule<parse_context_type>;

            class grammar : public rule_type {
            private:
                rule_type node;

            public:
                using rule_type::operator =;

                grammar() {
                    //newline is the LF character
                    const auto newline
                        = parserlib::newline('\n');

                    //recognizable whitespace is the space character and newline
                    const auto whitespace
                        = *(' ' | newline);

                    //special function that adds whitespace before parsing something;
                    //not strictly required, but makes the grammar code clearer
                    const auto text
                        = [=](auto&& p){ return whitespace >> make_parse_node(p); };

                    //letter
                    const auto letter
                        = range('a', 'z') | range('A', 'Z');

                    //digit
                    const auto digit
                        = range('0', '9');

                    //id
                    const auto id
                        = (letter | '_') >> *(letter | digit | '-' | '_');

                    //string literal; strings can be broken into lines
                    const auto string_literal
                        = '"' >> *((newline | any()) - '"') >> '"';

                    //attribute consists of attribute name and attribute value
                    const auto attribute
                        = (text(id)->*ID::ATTRIBUTE_NAME >> text('=') >> text(string_literal)->*ID::ATTRIBUTE_VALUE)->*ID::ATTRIBUTE;

                    //an attribute list consists of attributes
                    const auto attribute_list
                        = (*attribute)->*ID::ATTRIBUTE_LIST;

                    //the prolog; 'xml' is case insensitive
                    const auto prolog
                        = (text(case_insensitive_terminal("<?xml")) >> attribute_list >> text("?>"))->*ID::PROLOG;

                    //comment character; '--' is an error; comments can be broken into multiple lines
                    const auto comment_char 
                        = &text("--") >> error(ERROR_ID::INVALID_COMMENT_CHARACTERS, skip_after("--"))
                        | newline
                        | any()
                        ;

                    //a comment node
                    const auto comment_node
                        = text("<!--") >> *(comment_char - "-->") >> "-->";

                    //an element can end with '/>' or have content, then the closing tag
                    const auto element_node_end
                        = text("/>")
                        | text('>') >> (*node)->*ID::ELEMENT_CONTENT >> text("</") >> id->*ID::ELEMENT_CLOSING_TAG >> text(">");

                    //an element node; starts with '<'
                    const auto element_node 
                        = (text('<') >> id->*ID::ELEMENT_OPENING_TAG >> attribute_list >> element_node_end)->*ID::ELEMENT_NODE;

                    //a text node; any character except '<'
                    const auto text_node 
                        = text((+((newline | any()) - '<'))->*ID::TEXT_NODE);

                    //a node
                    node
                        = comment_node
                        | element_node
                        | text_node
                        ;

                    //the total content; the prolog is optional
                    *this = -prolog >> element_node >> text(end());
                }
            };
        };

        template <class Source>
        static auto parse(Source& source) {
            using parse_context_type = typename parser<Source>::parse_context_type;
            std::shared_ptr<parse_context_type> pc = std::make_shared<parse_context_type>(source);
            typename parser<Source>::grammar grammar;
            const bool success = grammar.parse(*pc);
            const auto ast = make_ast(pc->matches());
            return std::make_tuple(success, pc, ast);
        }
    };

} //namespace parserlib


#endif //PARSERLIB_XML_HPP
