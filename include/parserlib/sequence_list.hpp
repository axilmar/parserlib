#ifndef PARSERLIB__SEQUENCE_LIST_HPP
#define PARSERLIB__SEQUENCE_LIST_HPP


#include <vector>


namespace parserlib
{


    /**
        A parser that parses all elements of a list.
        @param T type of expression the list contains.
     */
    template <typename T> 
    class sequence_list : public expression
    {
    public:
        /**
            Constructor.
            @param list list of expressions.
         */
        sequence_list(std::vector<T>&& list)
            : m_list(std::move(list))
        {
        }

        /**
            Goes through each element of the list,
            and stops when an element fails to parse.
            @param pc parse context.
            @return parse result.
         */
        template <typename ParseContext>
        parse_result parse(ParseContext& pc) const
        {
            parse_result result = parse_result::rejected;
            for (size_t index = 0; index < m_list.size(); ++index)
            {
                result = m_list[index].parse(pc);
                if (result == parse_result::rejected)
                {
                    break;
                }
            }

            return result;
        }

    private:
        std::vector<T> m_list;
    };


} //namespace parserlib


#endif //PARSERLIB__SEQUENCE_LIST_HPP
