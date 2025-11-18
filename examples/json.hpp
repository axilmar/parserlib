#ifndef PARSERLIB_JSON_HPP
#define PARSERLIB_JSON_HPP


#include "parserlib.hpp"


#ifdef NULL
#undef NULL
#endif


//grammar taken from https://www.json.org/json-en.html


namespace parserlib {


    /**
     * Example JSON parser.
     */
    class json {
    public:
        /**
         * Token ID.
         */
        enum class TOKEN_ID {
            /** 'null' */
            NULL, 

            /** 'false' */
            FALSE,

            /** 'true' */
            TRUE,

            /** String */
            STRING,

            /** Number */
            NUMBER,

            /** ',' */
            COMMA,

            /** ':' */
            COLON,

            /** '{' */
            LEFT_CURLY_BRACKET,

            /** '}' */
            RIGHT_CURLY_BRACKET,

            /** [ */
            LEFT_SQUARE_BRACKET,

            /** ] */
            RIGHT_SQUARE_BRACKET
        };

        /**
         * AST ID.
         */
        enum class AST_ID {
            /** String */
            STRING,

            /** Number */
            NUMBER,

            /** Object */
            OBJECT,

            /** Member */
            MEMBER,

            /** Array */
            ARRAY,

            /** True */
            TRUE,

            /** False */
            FALSE,

            /** Null */
            NULL
        };

        /**
         * Error ID.
         */
        enum class ERROR_ID {
            INVALID_CHARACTERS,
        };

        template <class Source = std::string> class impl {
        public:
            /**
             * The tokenizer.
             */
            class tokenizer {
            public:
                /**
                 * The parse context for the tokenizer.
                 * The text position contains line and column.
                 */
                using parse_context_type = parse_context<std::string, TOKEN_ID, ERROR_ID, text_position>;

                /**
                 * The rule type for the tokenizer.
                 */
                using rule_type = rule<parse_context_type>;

                /**
                 * The grammar.
                 */
                class grammar : public rule_type {
                public:
                    using rule_type::operator =;

                    grammar() {
                        //set names
                        this->set_name("tokenizer");

                        //whitespace
                        const auto ws = set(0x09, 0x0d, 0x0a, 0x20);

                        //null terminal
                        const auto null_ = terminal("null")->*TOKEN_ID::NULL;

                        //false terminal
                        const auto false_ = terminal("false")->*TOKEN_ID::FALSE;

                        //true terminal
                        const auto true_ = terminal("true")->*TOKEN_ID::TRUE;

                        //comma
                        const auto comma = terminal(',')->*TOKEN_ID::COMMA;

                        //colon
                        const auto colon = terminal(':')->*TOKEN_ID::COLON;

                        //left curly bracket used for opening an object definition
                        const auto left_curly_bracket = terminal('{')->*TOKEN_ID::LEFT_CURLY_BRACKET;

                        //right curly bracket used for closing an object definition
                        const auto right_curly_bracket = terminal('}')->*TOKEN_ID::RIGHT_CURLY_BRACKET;

                        //left square bracket used for opening an array definition
                        const auto left_square_bracket = terminal('[')->*TOKEN_ID::LEFT_SQUARE_BRACKET;

                        //right square bracket used for closing an array definition
                        const auto right_square_bracket = terminal(']')->*TOKEN_ID::RIGHT_SQUARE_BRACKET;

                        //a digit
                        const auto digit = range('0', '9');

                        //a number (also accepts numbers like -3.5e+10)
                        const auto number = (-terminal('-') >> +digit >> -('.' >> +digit) >> -(-set("+-") >> +digit))->*TOKEN_ID::NUMBER;

                        //hex
                        const auto hex = range('0', '9') | range('a', 'f') | range('A', 'F');

                        //escape character
                        const auto escape_character
                            = terminal("\\\"")
                            | terminal("\\\\")
                            | terminal("\\/")
                            | terminal("\\b")
                            | terminal("\\f")
                            | terminal("\\n")
                            | terminal("\\r")
                            | terminal("\\t")
                            | "\\u" >> hex >> hex >> hex >> hex;
                        ;

                        //string character
                        const auto string_character
                            = range(0x20, 0x10ffff) - '"' - '\\'
                            | escape_character;

                        //string
                        const auto string = ('"' >> *string_character >> '"')->*TOKEN_ID::STRING;

                        //token
                        const auto token
                            = null_
                            | false_
                            | true_
                            | comma
                            | colon
                            | left_curly_bracket
                            | right_curly_bracket
                            | left_square_bracket
                            | right_square_bracket
                            | number
                            | string
                            ;

                        //symbol
                        const auto symbol
                            = newline('\n')
                            | ws
                            | token;

                        //on error, proceed to the next symbol
                        *this = *(symbol | error(ERROR_ID::INVALID_CHARACTERS, skip_before(symbol))) >> end();
                    }
                };
            };

            /**
             * The parser.
             */
            class parser {
            public:
                /**
                 * The parse context type uses the tokenizer output as source.
                 */
                using parse_context_type = parse_context<typename tokenizer::parse_context_type::match_container_type, AST_ID, ERROR_ID>;

                /**
                 * The rule type.
                 */
                using rule_type = rule<parse_context_type>;

                /**
                 * The grammar.
                 */
                class grammar : public rule_type {
                private:
                    //Recursive rules.
                    rule_type object, value;

                public:
                    using rule_type::operator =;

                    grammar() {
                        //set names
                        value.set_name("value");
                        object.set_name("object");
                        this->set_name("parser");

                        //string terminal
                        const auto string = terminal(TOKEN_ID::STRING)->*AST_ID::STRING;

                        //number terminal
                        const auto number = terminal(TOKEN_ID::NUMBER)->*AST_ID::NUMBER;

                        //array member list
                        const auto array_member_list = value >> *(TOKEN_ID::COMMA >> value);

                        //array
                        const auto array = (TOKEN_ID::LEFT_SQUARE_BRACKET >> -array_member_list >> TOKEN_ID::RIGHT_SQUARE_BRACKET)->*AST_ID::ARRAY;

                        //true terminal
                        const auto true_ = terminal(TOKEN_ID::TRUE)->*AST_ID::TRUE;

                        //false terminal
                        const auto false_ = terminal(TOKEN_ID::FALSE)->*AST_ID::FALSE;

                        //null terminal
                        const auto null_ = terminal(TOKEN_ID::NULL)->*AST_ID::NULL;

                        //the value
                        value
                            = string
                            | number
                            | object
                            | array
                            | true_
                            | false_
                            | null_
                            ;

                        //the object member
                        const auto object_member = (string >> TOKEN_ID::COLON >> value)->*AST_ID::MEMBER;

                        //the object member list
                        const auto object_member_list = object_member >> *(TOKEN_ID::COMMA >> object_member);

                        //the object
                        object = (TOKEN_ID::LEFT_CURLY_BRACKET >> -object_member_list >> TOKEN_ID::RIGHT_CURLY_BRACKET)->*AST_ID::OBJECT;

                        //the parser grammar
                        *this = object >> end();
                    }
                };
            };

            /**
             * The default ast factory type.
             */
            using default_ast_factory_type = default_ast_factory<typename parser::parse_context_type::source_type, typename parser::parse_context_type::match_id_type, typename parser::parse_context_type::text_position_type>;
        }; //class impl

        /**
         * The parse function.
         * @param source the source.
         * @parm astFactory the ast factory to use.
         * @return a `tokenize_and_parse_result` structure with the results of the parsing.
         */
        template <class Source, class ASTFactory>
        static auto parse(Source& source, const ASTFactory& astFactory) {
            using impl_type = json::impl<Source>;
            typename impl_type::tokenizer::grammar tokenizer_grammar;
            typename impl_type::parser::grammar parser_grammar;
            return tokenize_and_parse(source, tokenizer_grammar, parser_grammar, astFactory);
        }

        /**
         * The parse function with a default AST factory.
         * @param source the source.
         * @return a `tokenize_and_parse_result` structure with the results of the parsing.
         */
        template <class Source>
        static auto parse(Source& source) {
            return parse(source, typename impl<Source>::default_ast_factory_type());
        }
    };


    /**
     * Returns the name of a token id.
     * @param id id of token to get the name of.
     * @return the name of the given token id.
     */
    static const char* get_id_name(typename json::TOKEN_ID id) {
        static const char* names[] = {
            "NULL",
            "FALSE",
            "TRUE",
            "STRING",
            "NUMBER",
            "COMMA",
            "COLON",
            "LEFT_CURLY_BRACKET",
            "RIGHT_CURLY_BRACKET",
            "LEFT_SQUARE_BRACKET",
            "RIGHT_SQUARE_BRACKET"
        };
        return names[static_cast<int>(id)];
    }


    /**
     * Returns the name of an AST node id.
     * @param id id of AST node to get the name of.
     * @return the name of the given AST id.
     */
    static const char* get_id_name(typename json::AST_ID id) {
        static const char* names[] = {
            "STRING",
            "NUMBER",
            "OBJECT",
            "MEMBER",
            "ARRAY",
            "TRUE",
            "FALSE",
            "NULL"
        };
        return names[static_cast<int>(id)];
    }


    /**
     * Returns the name of an error id.
     * @param id id of the error to get the name of.
     * @return the name of the given error id.
     */
    static const char* get_id_name(typename json::ERROR_ID id) {
        static const char* names[] = {
            "INVALID_CHARACTERS",
        };
        return names[static_cast<int>(id)];
    }


} //namespace parserlib


#endif //PARSERLIB_JSON_HPP
