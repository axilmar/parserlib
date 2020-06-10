#ifndef PARSERLIB__EXAMPLES__JAVA__ERROR__HPP
#define PARSERLIB__EXAMPLES__JAVA__ERROR__HPP


#include <string>


namespace java
{


    ///error structure.
    struct Error
    {
        ///line.
        int line;

        ///column.
        int column;

        ///message.
        std::string message;

        ///constructor.
        Error(int l = 0, int c = 0, const std::string& s = std::string())
            : line(l), column(c), message(s)
        {
        }
    };


} //namespace java


#endif //PARSERLIB__EXAMPLES__JAVA__ERROR__HPP
