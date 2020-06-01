#ifndef PARSERLIB__PARSE_CONTEXT__HPP
#define PARSERLIB__PARSE_CONTEXT__HPP


#include <utility>
#include <string>


namespace parserlib
{


    /**
        Struct with data required for parsing.
        @param InputIterator type of input iterator.
     */
    template <typename InputIterator = std::string::const_iterator> 
    class parse_context
    {
    public:
        ///current position over the input.
        InputIterator iterator;

        ///input end.
        const InputIterator end;

        /**
            Constructor.
            @param container container to create a parse context out of.
            @return the parse context for parsing the input contained in the given container.
         */
        template <typename Container> 
        parse_context(Container&& container)
            : iterator(container.begin()), end(container.end())
        {
        }
    };


    /**
        Creates a parse context.
        @param container container to create a parse context out of.
        @return the parse context for parsing the input contained in the given container.
     */
    template <typename Container> 
    parse_context<Container> make_parse_context(Container&& container)
    {
        return std::forward<Container>(container);
    }


} //namespace parserlib


#endif //PARSERLIB__PARSE_CONTEXT__HPP
