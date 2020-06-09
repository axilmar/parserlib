#include "xml_grammar.hpp"


using namespace parserlib;


namespace xml
{

    const char *const TAG_NAME = "name";
    const char *const TAG_ATTRIBUTE_VALUE = "attribute_value";
    const char *const TAG_ATTRIBUTE = "attribute";
    const char *const TAG_ELEMENT = "element";
    const char *const TAG_CONTENT = "content";


    auto ws = *one_of(" \t\r\n\v");


    auto letter = range('a', 'z') | range('A', 'Z');


    auto digit = range('0', '9');


    auto any_char = range((char)1, (char)std::numeric_limits<char>::max());


    auto name = letter >> *(letter | digit) == TAG_NAME;


    auto attribute_value = *(!terminal('\"') >> any_char) == TAG_ATTRIBUTE_VALUE;


    auto attribute = name >> ws >> '=' >> ws >> '\"' >> attribute_value >> '\"' == TAG_ATTRIBUTE;

    
    auto element_opening_tag = '<' >> name >> *(ws >> attribute) >> ws >> '>';


    auto content = *(!terminal('<') >> any_char) == TAG_CONTENT;


    auto element_closing_tag = "</" >> name >> '>';


    rule<> element = element_opening_tag >> *(!terminal("</") >> (element | content)) >> element_closing_tag == TAG_ELEMENT;


    rule<> xml_grammar = ws >> element >> ws;


} //namespace xml
