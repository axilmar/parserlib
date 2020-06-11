#ifndef PARSERLIB__EXAMPLES__JAVA__TYPES__HPP
#define PARSERLIB__EXAMPLES__JAVA__TYPES__HPP


#include <string>


//fundamental types


namespace java
{


    //java character type
    typedef char16_t java_char;


    //java string type
    typedef std::u16string java_string;


    //error
    struct error
    {
        size_t line;
        size_t column;
        java_string message;
    };


} //namespace java


#endif //PARSERLIB__EXAMPLES__JAVA__TYPES__HPP
