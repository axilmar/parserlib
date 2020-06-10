#include <clocale>
#include <cwctype>
#include "tokenizer.hpp"


using namespace parserlib;

namespace java
{

    typedef parserlib::parse_context<std::u16string, token_type> java_parse_context;


    static const auto letter =
        terminal(&std::iswalpha) | '_' | '$';


    static const auto digit =
        one_of("0123456789");


    static const auto identifier
        = letter >> (letter | digit);


    static const auto grammar_token
        = terminal(u"abstract")     == token_type::KEYWORD_ABSTRACT
        | terminal(u"assert")       == token_type::KEYWORD_ASSERT
        | terminal(u"boolean")      == token_type::KEYWORD_BOOLEAN
        | terminal(u"break")        == token_type::KEYWORD_BREAK
        | terminal(u"byte")         == token_type::KEYWORD_BYTE
        | terminal(u"case")         == token_type::KEYWORD_CASE
        | terminal(u"catch")        == token_type::KEYWORD_CATCH
        | terminal(u"char")         == token_type::KEYWORD_CHAR
        | terminal(u"class")        == token_type::KEYWORD_CLASS
        | terminal(u"const")        == token_type::KEYWORD_CONST
        | terminal(u"continue")     == token_type::KEYWORD_CONTINUE
        | terminal(u"default")      == token_type::KEYWORD_DEFAULT
        | terminal(u"do")           == token_type::KEYWORD_DO
        | terminal(u"double")       == token_type::KEYWORD_DOUBLE
        | terminal(u"else")         == token_type::KEYWORD_ELSE
        | terminal(u"enum")         == token_type::KEYWORD_ENUM
        | terminal(u"extends")      == token_type::KEYWORD_EXTENDS
        | terminal(u"final")        == token_type::KEYWORD_FINAL
        | terminal(u"finally")      == token_type::KEYWORD_FINALLY
        | terminal(u"float")        == token_type::KEYWORD_FLOAT
        | terminal(u"for")          == token_type::KEYWORD_FOR
        | terminal(u"if")           == token_type::KEYWORD_IF
        | terminal(u"goto")         == token_type::KEYWORD_GOTO
        | terminal(u"implements")   == token_type::KEYWORD_IMPLEMENTS
        | terminal(u"import")       == token_type::KEYWORD_IMPORT
        | terminal(u"instanceof")   == token_type::KEYWORD_INSTANCEOF
        | terminal(u"int")          == token_type::KEYWORD_INT
        | terminal(u"interface")    == token_type::KEYWORD_INTERFACE
        | terminal(u"long")         == token_type::KEYWORD_LONG
        | terminal(u"native")       == token_type::KEYWORD_NATIVE
        | terminal(u"new")          == token_type::KEYWORD_NEW
        | terminal(u"package")      == token_type::KEYWORD_PACKAGE
        | terminal(u"private")      == token_type::KEYWORD_PRIVATE
        | terminal(u"protected")    == token_type::KEYWORD_PROTECTED
        | terminal(u"public")       == token_type::KEYWORD_PUBLIC
        | terminal(u"return")       == token_type::KEYWORD_RETURN
        | terminal(u"short")        == token_type::KEYWORD_SHORT
        | terminal(u"static")       == token_type::KEYWORD_STATIC
        | terminal(u"strictfp")     == token_type::KEYWORD_STRICTFP
        | terminal(u"super")        == token_type::KEYWORD_SUPER
        | terminal(u"switch")       == token_type::KEYWORD_SWITCH
        | terminal(u"synchronized") == token_type::KEYWORD_SYNCHRONIZED
        | terminal(u"this")         == token_type::KEYWORD_THIS
        | terminal(u"throw")        == token_type::KEYWORD_THROW
        | terminal(u"throws")       == token_type::KEYWORD_THROWS
        | terminal(u"transient")    == token_type::KEYWORD_TRANSIENT
        | terminal(u"try")          == token_type::KEYWORD_TRY
        | terminal(u"void")         == token_type::KEYWORD_VOID
        | terminal(u"volatile")     == token_type::KEYWORD_VOLATILE
        | terminal(u"while")        == token_type::KEYWORD_WHILE

        | identifier == token_type::IDENTIFIER;


    static const auto grammar
        = *grammar_token;


    //tokenize input
    std::vector<token> tokenize(const java_string& input, std::vector<Error>& errors)
    {
        std::vector<token> result;

        //set locale to utf-16
        std::setlocale(LC_ALL, "en_US.UTF-16");

        return result;
    }


} //namespace java
