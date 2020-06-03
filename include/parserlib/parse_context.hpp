#ifndef PARSERLIB__PARSE_CONTEXT__HPP
#define PARSERLIB__PARSE_CONTEXT__HPP


#include <utility>
#include <string>
#include <map>
#include <vector>
#include <string_view>


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

        ///match.
        struct match
        {
            ///matched input.
            typename std::basic_string_view<typename Input::value_type> input;

            ///tag.
            std::string_view tag;
        };

        ///state
        struct state
        {
            ///current position over the input.
            typename Input::const_iterator position;

            ///matches container size.
            size_t matches_size;
        };

        ///left recursion data.
        struct left_recursion
        {
            ///left recursion state.
            left_recursion_state state = left_recursion_state::inactive;

            ///position left recursion is currently at.
            typename Input::const_iterator position;
        };

        ///map used in handling recursion.
        std::map<const rule<parse_context>*, std::vector<typename Input::const_iterator>> positions;

        ///input begin.
        const typename Input::const_iterator begin;

        ///input end.
        const typename Input::const_iterator end;

        ///current position over the input.
        typename Input::const_iterator position;

        ///left recursion data.
        left_recursion left_recursion;

        ///matches.
        std::vector<match> matches;

        /**
            Constructor.
            @param container container to create a parse context out of.
            @return the parse context for parsing the input contained in the given container.
         */
        parse_context(const Input& container)
            : begin(container.begin())
            , position(container.begin())
            , end(container.end())
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
            return { position, matches.size() };
        }

        /**
            Sets the current state.
            @param s state.
         */
        void set_state(const state& s)
        {
            position = s.position;
            matches.resize(s.matches_size);
        }

        /**
            Returns the remaining input.
            @return the remaining input.
         */
		Input get_remaining_input() const
		{
			return Input(position, end);
		}

        /**
            Helper function for adding a match.
            @param begin input begin.
            @param end input end.
            @param tag input tag.
         */
        void add_match(const typename Input::const_iterator begin, const typename Input::const_iterator end, const std::string_view& tag)
        {
            matches.push_back(match{ {&*begin, (size_t)std::distance(begin, end)}, tag });
        }
    };


} //namespace parserlib


#endif //PARSERLIB__PARSE_CONTEXT__HPP
