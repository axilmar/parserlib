#ifndef PARSERLIB__EXAMPLES__XML__XML_GRAMMAR__HPP
#define PARSERLIB__EXAMPLES__XML__XML_GRAMMAR__HPP


#include "parserlib.hpp"


/*
    DISCLAIMER: this is not a fully-fledged XML parser;
    it only parses elements, attributes and content.
    It is only provided as an example.
 */


namespace xml
{


    extern const char *const TAG_NAME;
    extern const char *const TAG_ATTRIBUTE_VALUE;
    extern const char *const TAG_ATTRIBUTE;
    extern const char *const TAG_ELEMENT;
    extern const char *const TAG_CONTENT;


    extern parserlib::rule<> xml_grammar;


} //namespace xml


#endif //PARSERLIB__EXAMPLES__XML__XML_GRAMMAR__HPP
