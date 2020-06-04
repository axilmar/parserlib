#ifndef PARSERLIB__PARSE_CONTEXT__HPP
#define PARSERLIB__PARSE_CONTEXT__HPP


#include <utility>
#include <string>
#include <string_view>
#include <vector>
#include <ostream>
#include <map>


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
            ///begin of match input.
            typename Input::const_iterator begin;

            ///end of match input.
            typename Input::const_iterator end;

            ///tag.
            std::string_view tag;

            ///automatic conversion to string.
            operator std::basic_string<typename Input::value_type> () const
            {
                return { begin, end };
            }

            /**
                Operator used for match output to a stream.
                @param stream output stream.
                @param m match.
                @return the output stream.
             */
            template <typename Char, typename Traits>
            friend std::basic_ostream<Char, Traits>& operator << (std::basic_ostream<Char, Traits>& stream, const match &m)
            {
                for (auto it = m.begin; it != m.end; ++it)
                {
                    stream << *it;
                }
                return stream;
            }
        };

        ///state
        struct state
        {
            ///current position over the input.
            typename Input::const_iterator position;

            ///matches container size.
            size_t matches_size;
        };

        ///current position over the input.
        typename Input::const_iterator position;

        ///left recursion data.
        struct left_recursion
        {
            ///state.
            left_recursion_state state = left_recursion_state::inactive;

            ///current left recursion position.
            typename Input::const_iterator position;
        } left_recursion;

        ///input begin.
        const typename Input::const_iterator begin;

        ///input end.
        const typename Input::const_iterator end;

        ///rule positions.
        std::map<const rule<parse_context>*, std::vector<typename Input::const_iterator>> rule_positions;

        ///matches.
        std::vector<match> matches;

        /**
            Constructor.
            @param container container to create a parse context out of.
            @return the parse context for parsing the input contained in the given container.
         */
        parse_context(const Input& container)
            : position(container.begin())
            , left_recursion{ left_recursion_state::inactive, container.begin() }
            , begin(container.begin())
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
        struct state state() const
        {
            return { position, matches.size() };
        }

        /**
            Sets the current state.
            @param s state.
         */
        void set_state(const struct state& s)
        {
            position = s.position;
            matches.resize(s.matches_size);
        }

        /**
            Returns the remaining input.
            @return the remaining input.
         */
		Input remaining_input() const
		{
			return Input(position, end);
		}

        /**
            Helper function for adding a match.
            @param begin start of matched input.
            @param end end of matched input.
            @param tag input tag.
         */
        void add_match(
            const typename Input::const_iterator begin, 
            const typename Input::const_iterator end, 
            const std::string_view& tag)
        {
            matches.push_back(match{ begin, end, tag });
        }

        /**
            Adds the current position for the given rule.
            @param rule rule to add the current position for.
            @return true if the two last positions are the same (useful in recognizing left recursion), false otherwise.

         */
        bool add_position(const rule<parse_context>* rule)
        {
            auto& pos = rule_positions[rule];
            pos.push_back(position);
            return pos.size() >= 2 && pos.back() == *(pos.end() - 2);
        }

        /**
            Removes the last known position for a rule.
            @param rule rule to remove the position of.
         */
        void remove_position(const rule<parse_context>* rule)
        {
            rule_positions[rule].pop_back();
        }
    };


} //namespace parserlib


#endif //PARSERLIB__PARSE_CONTEXT__HPP
