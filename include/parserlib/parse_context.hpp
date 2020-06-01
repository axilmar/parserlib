#ifndef PARSERLIB__PARSE_CONTEXT__HPP
#define PARSERLIB__PARSE_CONTEXT__HPP


#include <utility>
#include <string>
#include <map>
#include <vector>


namespace parserlib
{


    template <typename ParseContext> class rule;


    /**
        Struct with data required for parsing.
        @param Input input type.
     */
    template <typename Input = std::string> 
    class parse_context
    {
    public:
        ///input type.
        typedef Input input_type;

        ///state
        struct state
        {
            ///position over the input.
            typename Input::const_iterator iterator;
        };

        ///map used in handling recursion.
        std::map<const rule<parse_context>*, std::vector<typename Input::const_iterator>> positions;

        ///current position over the input.
        typename Input::const_iterator iterator;

        ///input end.
        const typename Input::const_iterator end;

        /**
            Constructor.
            @param container container to create a parse context out of.
            @return the parse context for parsing the input contained in the given container.
         */
        parse_context(const Input& container)
            : iterator(container.begin()), end(container.end())
        {
        }

        /**
            Checks if iterator has reached end.
            @return true if iterator has not reached end, false otherwise.
         */
        bool valid() const
        {
            return iterator < end;
        }

        /**
            Returns the current state.
            @return state.
         */
        state get_state() const
        {
            return { iterator };
        }

        /**
            Sets the current state.
            @param s state.
         */
        void set_state(const state& s)
        {
            iterator = s.iterator;
        }

        /**
            Returns the remaining input.
            @return the remaining input.
         */
		Input get_remaining_input() const
		{
			return Input(iterator, end);
		}

        /**
            Proceeds with next token.
         */
        void next()
        {
            ++iterator;
        }
    };


    /**
        Creates a parse context.
        @param container container to create a parse context out of.
        @return the parse context for parsing the input contained in the given container.
     */
    template <typename Input> 
    parse_context<Input> make_parse_context(const Input& container)
    {
        return  { container };
    }


} //namespace parserlib


#endif //PARSERLIB__PARSE_CONTEXT__HPP
