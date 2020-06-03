#ifndef PARSERLIB__PARSE_RESULT__HPP
#define PARSERLIB__PARSE_RESULT__HPP


namespace parserlib
{


    /**
        Possible results of the parse function.
     */
    enum class parse_result
    {
        ///The input was parsed successfully.
        accepted,

        ///The input was rejected.
        rejected
    };


} //namespace parserlib


#endif //PARSERLIB__PARSE_RESULT__HPP
