#ifndef PARSERLIB_ENDOF_HPP
#define PARSERLIB_ENDOF_HPP


namespace parserlib
{


    /**
        Locates the end of the given null-terminated sequence.
        @param s sequence.
        @return the end of sequence where the null-terminated symbol is found.
     */
    template <typename SymbolType>
    static const SymbolType* endOf(const SymbolType* s)
    {
        for (; *s; ++s) {}
        return s;
    }


} //namespace parserlib


#endif //PARSERLIB_ENDOF_HPP
