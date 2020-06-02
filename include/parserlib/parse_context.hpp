#ifndef PARSERLIB__PARSE_CONTEXT__HPP
#define PARSERLIB__PARSE_CONTEXT__HPP


#include <utility>
#include <string>
#include <map>
#include <vector>


namespace parserlib
{


    template <typename ParseContext> class rule;


    enum class left_recursion_state
    {
        inactive,
        reject,
        accept
    };


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
            typename Input::const_iterator position;
        };

        ///map used in handling recursion.
        std::map<const rule<parse_context>*, std::vector<typename Input::const_iterator>> positions;

        ///current position over the input.
        typename Input::const_iterator position;

        ///input end.
        const typename Input::const_iterator end;

        ///left recursion data.
        struct left_recursion
        {
            ///left recursion state.
            left_recursion_state state = left_recursion_state::inactive;

            ///position left recursion is currently at.
            typename Input::const_iterator position;
        } left_recursion;

        /**
            Constructor.
            @param container container to create a parse context out of.
            @return the parse context for parsing the input contained in the given container.
         */
        parse_context(const Input& container)
            : position(container.begin()), end(container.end())
        {
        }

        /**
            Checks if position has reached end.
            @return true if position has not reached end, false otherwise.
         */
        bool valid() const
        {
            return position < end;
        }

        /**
            Returns the current state.
            @return state.
         */
        state get_state() const
        {
            return { position };
        }

        /**
            Sets the current state.
            @param s state.
         */
        void set_state(const state& s)
        {
            position = s.position;
        }

        /**
            Returns the remaining input.
            @return the remaining input.
         */
		Input get_remaining_input() const
		{
			return Input(position, end);
		}
    };


} //namespace parserlib


#endif //PARSERLIB__PARSE_CONTEXT__HPP
