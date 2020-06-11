#ifndef PARSERLIB__EXAMPLES__JAVA__PREPROCESSOR__HPP
#define PARSERLIB__EXAMPLES__JAVA__PREPROCESSOR__HPP


#include <vector>
#include "types.hpp"


namespace java
{


    /*
        Preprocesses java input:
        1) handles unicode escapes.
        2) separates the code into lines.
     */
    std::vector<java_string> preprocess(const java_string& input, std::vector<error>& errors);


} //namespace java


#endif //PARSERLIB__EXAMPLES__JAVA__PREPROCESSOR__HPP
