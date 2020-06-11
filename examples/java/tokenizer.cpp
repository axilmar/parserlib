#include <algorithm>
#include <cwctype>
#include <clocale>
#include "parserlib.hpp"
#include "preprocessor.hpp"
#include "tokenizer.hpp"


using namespace parserlib;


namespace java
{


    //concatenate vectors
    template <typename T>
    static std::vector<T> concatenate(const std::vector<T>& a, const std::vector<T>& b, const std::vector<T>& c)
    {
        std::vector<T> result;
        result.insert(result.end(), a.begin(), a.end());
        result.insert(result.end(), b.begin(), b.end());
        result.insert(result.end(), c.begin(), c.end());
        return std::move(result);
    }


    //sorts vector of java string entries by string length; longest go first.
    static std::vector<std::pair<java_string, token_type>> sort_by_length(std::vector<std::pair<java_string, token_type>>&& strings)
    {
        std::sort(strings.begin(), strings.end(), [](const std::pair<java_string, token_type>& a, const std::pair<java_string, token_type>& b)
        {
            if (a.first.length() > b.first.length()) return true;
            if (a.first.length() < b.first.length()) return false;
            return a.first < b.first;
        });
        return std::move(strings);
    }


    //sorts vector of java string entries by string length; longest go first.
    static std::vector<std::pair<java_string, token_type>> sort_by_length(const std::vector<std::pair<java_string, token_type>>& strings)
    {
        std::vector<std::pair<java_string, token_type>> temp = strings;
        return sort_by_length(std::move(temp));
    }


    //keywords
    static const std::vector<std::pair<java_string, token_type>> keywords = 
        std::vector<std::pair<java_string, token_type>>
        {
            std::pair<java_string, token_type>(u"abstract"     , token_type::KEYWORD_ABSTRACT),
            std::pair<java_string, token_type>(u"assert"       , token_type::KEYWORD_ASSERT),  
            std::pair<java_string, token_type>(u"boolean"      , token_type::KEYWORD_BOOLEAN), 
            std::pair<java_string, token_type>(u"break"        , token_type::KEYWORD_BREAK),   
            std::pair<java_string, token_type>(u"byte"         , token_type::KEYWORD_BYTE),    
            std::pair<java_string, token_type>(u"case"         , token_type::KEYWORD_CASE),    
            std::pair<java_string, token_type>(u"catch"        , token_type::KEYWORD_CATCH),   
            std::pair<java_string, token_type>(u"char"         , token_type::KEYWORD_CHAR),    
            std::pair<java_string, token_type>(u"class"        , token_type::KEYWORD_CLASS),   
            std::pair<java_string, token_type>(u"const"        , token_type::KEYWORD_CONST),   
            std::pair<java_string, token_type>(u"continue"     , token_type::KEYWORD_CONTINUE),
            std::pair<java_string, token_type>(u"default"      , token_type::KEYWORD_DEFAULT), 
            std::pair<java_string, token_type>(u"do"           , token_type::KEYWORD_DO),      
            std::pair<java_string, token_type>(u"double"       , token_type::KEYWORD_DOUBLE),  
            std::pair<java_string, token_type>(u"else"         , token_type::KEYWORD_ELSE),    
            std::pair<java_string, token_type>(u"enum"         , token_type::KEYWORD_ENUM),    
            std::pair<java_string, token_type>(u"extends"      , token_type::KEYWORD_EXTENDS), 
            std::pair<java_string, token_type>(u"final"        , token_type::KEYWORD_FINAL),   
            std::pair<java_string, token_type>(u"finally"      , token_type::KEYWORD_FINALLY), 
            std::pair<java_string, token_type>(u"float"        , token_type::KEYWORD_FLOAT),   
            std::pair<java_string, token_type>(u"for"          , token_type::KEYWORD_FOR),       
            std::pair<java_string, token_type>(u"if"           , token_type::KEYWORD_IF),        
            std::pair<java_string, token_type>(u"goto"         , token_type::KEYWORD_GOTO),      
            std::pair<java_string, token_type>(u"implements"   , token_type::KEYWORD_IMPLEMENTS),
            std::pair<java_string, token_type>(u"import"       , token_type::KEYWORD_IMPORT),    
            std::pair<java_string, token_type>(u"instanceof"   , token_type::KEYWORD_INSTANCEOF),
            std::pair<java_string, token_type>(u"int"          , token_type::KEYWORD_INT),       
            std::pair<java_string, token_type>(u"interface"    , token_type::KEYWORD_INTERFACE), 
            std::pair<java_string, token_type>(u"long"         , token_type::KEYWORD_LONG),      
            std::pair<java_string, token_type>(u"native"       , token_type::KEYWORD_NATIVE),    
            std::pair<java_string, token_type>(u"new"          , token_type::KEYWORD_NEW),       
            std::pair<java_string, token_type>(u"package"      , token_type::KEYWORD_PACKAGE),   
            std::pair<java_string, token_type>(u"private"      , token_type::KEYWORD_PRIVATE),   
            std::pair<java_string, token_type>(u"protected"    , token_type::KEYWORD_PROTECTED), 
            std::pair<java_string, token_type>(u"public"       , token_type::KEYWORD_PUBLIC),    
            std::pair<java_string, token_type>(u"return"       , token_type::KEYWORD_RETURN),    
            std::pair<java_string, token_type>(u"short"        , token_type::KEYWORD_SHORT),     
            std::pair<java_string, token_type>(u"static"       , token_type::KEYWORD_STATIC),    
            std::pair<java_string, token_type>(u"strictfp"     , token_type::KEYWORD_STRICTFP),  
            std::pair<java_string, token_type>(u"super"        , token_type::KEYWORD_SUPER),     
            std::pair<java_string, token_type>(u"switch"       , token_type::KEYWORD_SWITCH),
            std::pair<java_string, token_type>(u"synchronized" , token_type::KEYWORD_SYNCHRONIZED),
            std::pair<java_string, token_type>(u"this"         , token_type::KEYWORD_THIS),
            std::pair<java_string, token_type>(u"throw"        , token_type::KEYWORD_THROW),
            std::pair<java_string, token_type>(u"throws"       , token_type::KEYWORD_THROWS),
            std::pair<java_string, token_type>(u"transient"    , token_type::KEYWORD_TRANSIENT),
            std::pair<java_string, token_type>(u"try"          , token_type::KEYWORD_TRY),
            std::pair<java_string, token_type>(u"void"         , token_type::KEYWORD_VOID),
            std::pair<java_string, token_type>(u"volatile"     , token_type::KEYWORD_VOLATILE),
            std::pair<java_string, token_type>(u"while"        , token_type::KEYWORD_WHILE),
            std::pair<java_string, token_type>(u"_"            , token_type::KEYWORD_UNDERSCORE)
        };


    //separators
    static const std::vector<std::pair<java_string, token_type>> separators =
        std::vector<std::pair<java_string, token_type>>
        {
            std::pair<java_string, token_type>(u"(" , token_type::SEPARATOR_LEFT_PARENTHESIS),
            std::pair<java_string, token_type>(u")" , token_type::SEPARATOR_RIGHT_PARENTHESIS),
            std::pair<java_string, token_type>(u"{" , token_type::SEPARATOR_LEFT_CURLY_BRACKET),
            std::pair<java_string, token_type>(u"}" , token_type::SEPARATOR_RIGHT_CURLY_BRACKET),
            std::pair<java_string, token_type>(u"[" , token_type::SEPARATOR_LEFT_SQUARE_BRACKET),
            std::pair<java_string, token_type>(u"]" , token_type::SEPARATOR_RIGHT_SQUARE_BRACKET),
            std::pair<java_string, token_type>(u";" , token_type::SEPARATOR_SEMICOLON),
            std::pair<java_string, token_type>(u"," , token_type::SEPARATOR_COMMA),
            std::pair<java_string, token_type>(u"." , token_type::SEPARATOR_DOT)
        };


    //operators
    static const std::vector<std::pair<java_string, token_type>> operators = 
        std::vector<std::pair<java_string, token_type>>
        {
            std::pair<java_string, token_type>(u"="   , token_type::OPERATOR_ASSIGN),
            std::pair<java_string, token_type>(u"<"   , token_type::OPERATOR_LESS_THAN),
            std::pair<java_string, token_type>(u">"   , token_type::OPERATOR_GREATER_THAN),
            std::pair<java_string, token_type>(u"!"   , token_type::OPERATOR_LOGICAL_NOT),
            std::pair<java_string, token_type>(u"~"   , token_type::OPERATOR_BITWISE_NOT),
            std::pair<java_string, token_type>(u"?"   , token_type::OPERATOR_IF),
            std::pair<java_string, token_type>(u":"   , token_type::OPERATOR_COLON),
            std::pair<java_string, token_type>(u"=="  , token_type::OPERATOR_EQUAL_TO),
            std::pair<java_string, token_type>(u"<="  , token_type::OPERATOR_LESS_THAN_OR_EQUAL_TO),
            std::pair<java_string, token_type>(u">="  , token_type::OPERATOR_GREATER_THAN_OR_EQUAL_TO),
            std::pair<java_string, token_type>(u"!="  , token_type::OPERATOR_DIFFERENT_THAN),
            std::pair<java_string, token_type>(u"&&"  , token_type::OPERATOR_LOGICAL_AND),
            std::pair<java_string, token_type>(u"||"  , token_type::OPERATOR_LOGICAL_OR),
            std::pair<java_string, token_type>(u"++"  , token_type::OPERATOR_INCREMENT),
            std::pair<java_string, token_type>(u"--"  , token_type::OPERATOR_DECREMENT),
            std::pair<java_string, token_type>(u"+"   , token_type::OPERATOR_ADD),
            std::pair<java_string, token_type>(u"-"   , token_type::OPERATOR_SUBTRACT),
            std::pair<java_string, token_type>(u"*"   , token_type::OPERATOR_MULTIPLY),
            std::pair<java_string, token_type>(u"/"   , token_type::OPERATOR_DIVIDE),
            std::pair<java_string, token_type>(u"&"   , token_type::OPERATOR_BITWISE_AND),
            std::pair<java_string, token_type>(u"|"   , token_type::OPERATOR_BITWISE_OR),
            std::pair<java_string, token_type>(u"^"   , token_type::OPERATOR_BITWISE_XOR),
            std::pair<java_string, token_type>(u"%"   , token_type::OPERATOR_MODULO),
            std::pair<java_string, token_type>(u"<<"  , token_type::OPERATOR_LEFT_SHIFT),
            std::pair<java_string, token_type>(u">>"  , token_type::OPERATOR_RIGHT_SHIFT),
            std::pair<java_string, token_type>(u">>>" , token_type::OPERATOR_SIGNED_RIGHT_SHIFT),
            std::pair<java_string, token_type>(u"+="  , token_type::OPERATOR_ADD_ASSIGN),
            std::pair<java_string, token_type>(u"-="  , token_type::OPERATOR_SUBTRACT_ASSIGN),
            std::pair<java_string, token_type>(u"*="  , token_type::OPERATOR_MULTIPLY_ASSIGN),
            std::pair<java_string, token_type>(u"/="  , token_type::OPERATOR_DIVIDE_ASSIGN),
            std::pair<java_string, token_type>(u"&="  , token_type::OPERATOR_BITWISE_AND_ASSIGN),
            std::pair<java_string, token_type>(u"|="  , token_type::OPERATOR_BITWISE_OR_ASSIGN),
            std::pair<java_string, token_type>(u"^="  , token_type::OPERATOR_BITWISE_XOR_ASSIGN),
            std::pair<java_string, token_type>(u"%="  , token_type::OPERATOR_BITWISE_MODULO_ASSIGN),
            std::pair<java_string, token_type>(u"<<=" , token_type::OPERATOR_LEFT_SHIFT_ASSIGN),
            std::pair<java_string, token_type>(u">>=" , token_type::OPERATOR_RIGHT_SHIFT_ASSIGN),
            std::pair<java_string, token_type>(u">>>=", token_type::OPERATOR_SIGNED_RIGHT_SHIFT_ASSIGN)
        };


    //all static input elements, sorted by length
    static const std::vector<std::pair<java_string, token_type>> static_input_elements =
        concatenate(keywords, separators, operators);


    //creates the appropriate grammar for the given list of elements.
    static auto create_input_elements_grammar(const std::vector<std::pair<java_string, token_type>>& elements)
    {
        const std::vector<std::pair<java_string, token_type>> sorted_elements = sort_by_length(elements);        
        typedef match<terminal<java_string>, token_type> element_grammar_type;        
        std::vector<element_grammar_type> grammar;
        for (const auto& p : sorted_elements)
        {
            grammar.push_back(element_grammar_type(terminal(p.first), token_type(p.second)));
        }
        return choice_list<element_grammar_type>(std::move(grammar));
    }


    static const auto whitespace = +one_of(u" \t\f");


    static const auto any_char = range(java_char(0), std::numeric_limits<java_char>::max());


    static const auto traditional_comment
        = u"/*" >> *(!terminal(u"*/") >> any_char) >> u"*/";


    static const auto end_of_line_comment
        = u"//" >> (!eof >> *any_char) >> eof;


    static const auto comment
        = traditional_comment
        | end_of_line_comment;


    static const auto integer_type_suffix = one_of(u"lL");


    static const auto non_zero_digit = one_of(u"123456789");


    static const auto underscores = +terminal(u'_');


    static const auto digit = one_of(u"0123456789");


    static const auto digits = digit >> *(*terminal(u'_') >> digit);


    static const auto decimal_numeral
        = non_zero_digit >> underscores >> digits
        | non_zero_digit >> -digits
        | u'0';


    static const auto decimal_integer_literal
        = decimal_numeral >> -integer_type_suffix;


    static const auto integer_literal
        = decimal_integer_literal;


    static const auto literal
        = integer_literal == token_type::LITERAL_INTEGER;


    static const auto letter
        = terminal(&std::iswalpha)
        | u'_'
        | u'$';


    static const auto identifier = letter >> (letter | digit);


    static const auto token_
        = create_input_elements_grammar(static_input_elements)
        | literal
        | identifier == token_type::IDENTIFIER;


    static const auto input_element
        = whitespace
        | comment
        | token_;


    //tokenizer grammar
    static const auto grammar = *input_element;


    //tokenize line
    static void tokenize(const java_string& input, const size_t line, std::vector<error>& errors, std::vector<token> &result)
    {
        parse_context<java_string, token_type> pc(input);

        while (pc.valid())
        {
            const bool parse_ok = parse(grammar, pc);
            
            if (!parse_ok)
            {
                const size_t col = (size_t)(pc.position - input.begin());
                if (errors.empty() || col - errors.back().column > 1)
                {
                    errors.push_back(error{ line, col, u"invalid token" });
                }
                pc.position = pc.furthest_position;
                continue;
            }

            for (const auto& match : pc.matches)
            {
                const size_t col = (size_t)(match.begin - input.begin());
                result.push_back(token{ match.tag, line, col, java_string(match.begin, match.end)});
            }
        }
    }


    //tokenize lines
    static std::vector<token> tokenize(const std::vector<java_string>& input, std::vector<error>& errors)
    {
        std::vector<token> result;
        for (size_t index = 0; index < input.size(); ++index)
        {
            tokenize(input[index], index, errors, result);
        }
        return std::move(result);
    }


    //tokenize input
    std::vector<token> tokenize(const java_string& input, std::vector<error>& errors)
    {
        std::setlocale(LC_ALL, "en_US.utf16");
        std::vector<java_string> lines = preprocess(input, errors);
        return tokenize(lines, errors);
    }


} //namespace java
