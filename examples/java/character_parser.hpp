#ifndef PARSERLIB__EXAMPLES__JAVA__CHARACTER_PARSER__HPP
#define PARSERLIB__EXAMPLES__JAVA__CHARACTER_PARSER__HPP


#include <string>
#include "parserlib.hpp"
#include "Error.hpp"


namespace java
{


    ///java string.
    typedef std::u16string java_string;


    /**
        Parses the given input into a java string.
        What it basically does is to correctly parse input
        into a java string, i.e. converting unicode characters,
        making sure \uuuu... is correctly handled etc.
        @param input input data.
        @param errors error list.
        @return output string.
     */
    java_string parse_characters(const std::u16string& input, std::vector<Error>& errors);


} //namespace java


#endif //PARSERLIB__EXAMPLES__JAVA__CHARACTER_PARSER__HPP
