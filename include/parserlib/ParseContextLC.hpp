#ifndef PARSERLIB_PARSECONTEXTLC_HPP
#define PARSERLIB_PARSECONTEXTLC_HPP


#include "SourceView.hpp"
#include "ParseContext.hpp"


namespace parserlib {


    /**
     * Enhances the ParseContext class with line and column counting capabilities.
     * @param SourceType container with source data;
     *  must have an STL container interface;
     *  must outlive the parser context;
     *  must be immutable while being used by a parser context.
     * @param MatchIdType id to apply to a match.
     * @param WSParserType parser to use for parsing whitespace; defaults to EmptyParser.
     * @param NL newline traits.
     */
    template <class SourceType = std::string, class MatchIdType = std::string, class WSParserType = EmptyParser, class NLTraits = NewlineTraits>
    class ParseContextLC 
        : public SourceView<SourceType, NLTraits>
        , public ParseContext<SourceView<SourceType, NLTraits>, MatchIdType, WSParserType>
    {
    public:
        /**
         * The constructor.
         * @param src source.
         * @param ws whitespace parser.
         */
        ParseContextLC(const SourceType& src, const WSParserType& ws = WSParserType{}) 
            : SourceView<SourceType, NLTraits>(src)
            , ParseContext<SourceView<SourceType, NLTraits>, MatchIdType, WSParserType>(*this, ws)
        {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSECONTEXTLC_HPP
