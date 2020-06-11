#ifndef PARSERLIB__EOF__HPP
#define PARSERLIB__EOF__HPP


#include "expression_type.hpp"


namespace parserlib
{


    /**        
        A parser that returns true if input has ended.
     */
    class eof : public expression
    {
    public:
        /**
            Parses the given expression.
            @param pc parse context.
            @return 'accepted' if end of input has been reached.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            return pc.valid() ? parse_result::rejected : parse_result::accepted;
        }
    };


    ///the one and only instance of eof.
    inline constexpr class eof eof;


} //namespace parserlib


#endif //PARSERLIB__EOF__HPP
