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

        ///input begin.
        const typename Input::const_iterator begin;

        ///input end.
        const typename Input::const_iterator end;

        ///parse positions.
        std::map<const void*, std::vector<typename Input::const_iterator>> parse_positions;

        ///matches.
        std::vector<match> matches;

        /**
            Constructor.
            @param container container to create a parse context out of.
            @return the parse context for parsing the input contained in the given container.
         */
        parse_context(const Input& container)
            : position(container.begin())
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
            Helper function for adding a parse position.
            @param obj object to add a parse position for.
            @return true if the two last positions are the same (useful in recognizing left recursion), false otherwise.

         */
        bool add_position(const void* obj)
        {
            auto& pos = parse_positions[obj];
            pos.push_back(position);
            return pos.size() >= 2 && pos.back() == *(pos.end() - 2);
        }

        /**
            Removes the last parse position.
            @param obj obj to remove the last parse position of.
         */
        void remove_position(const void* obj)
        {
            parse_positions[obj].pop_back();
        }
    };


} //namespace parserlib


#endif //PARSERLIB__PARSE_CONTEXT__HPP
