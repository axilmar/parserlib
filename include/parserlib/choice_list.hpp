#ifndef PARSERLIB__CHOICE_LIST_HPP
#define PARSERLIB__CHOICE_LIST_HPP


#include <vector>


namespace parserlib
{


    /**
        A parser that parses all elements of a list, until one succeeds.
        @param T type of expression the list contains.
     */
    template <typename T> 
    class choice_list : public expression
    {
    public:
        /**
            Constructor.
            @param list list of expressions.
         */
        choice_list(std::vector<T>&& list)
            : m_list(std::move(list))
        {
        }

        /**
            Goes through each element of the list,
            and stops when an element parses successfully.
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

                if (result == parse_result::accepted)
                {
                    if (pc.m_left_recursion_state == ParseContext::left_recursion_state::reject)
                    {
                        while (pc.valid())
                        {
                            parse_result result1 = parse_result::rejected;
                            
                            for (size_t index1 = 0; index1 < index; ++index1)
                            {
                                pc.m_left_recursion_state = ParseContext::left_recursion_state::accept;
                                pc.m_left_recursion_position = pc.position;
                                
                                result1 = m_list[index1].parse(pc);
                                
                                if (result1 == parse_result::accepted)
                                {
                                    break;
                                }
                            }

                            if (result1 == parse_result::rejected)
                            {
                                break;
                            }
                        }

                        pc.m_left_recursion_state = ParseContext::left_recursion_state::reject;
                    }
                    
                    break;
                }
            }

            return result;
        }

    private:
        std::vector<T> m_list;
    };


} //namespace parserlib


#endif //PARSERLIB__CHOICE_LIST_HPP
