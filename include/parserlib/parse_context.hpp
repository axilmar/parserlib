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


    template <typename L, typename R> class choice;
    template <typename T> class choice_list;
    template <typename ParseContext> class rule;



    /**
        Struct with data required for parsing.
        @param Input input type.
        @param Tag tag type.
        @param Output output type.
     */
    template <typename Input = std::string, typename Tag = std::string_view, typename Output = std::string> 
    class parse_context
    {
    public:
        ///input type.
        typedef Input input_type;

        ///tag type.
        typedef Tag tag_type;

        ///output type.
        typedef Output output_type;

        ///match.
        struct match
        {
            ///match tag.
            Tag tag;

            ///begin of match input.
            typename Input::const_iterator begin;

            ///end of match input.
            typename Input::const_iterator end;

            ///automatic conversion to string.
            std::basic_string<typename Input::value_type> input() const
            {
                return { static_cast<Output::const_iterator>(begin), static_cast<Output::const_iterator>(end) };
            }

            ///automatic conversion to string.
            operator std::basic_string<typename Input::value_type> () const
            {
                return input();
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

        ///input begin.
        const typename Input::const_iterator begin;

        ///input end.
        const typename Input::const_iterator end;

        ///current position over the input.
        typename Input::const_iterator position;

        ///furthest position reached.
        typename Input::const_iterator furthest_position;

        ///matches.
        std::vector<match> matches;

        /**
            Constructor.
            @param container container to create a parse context out of.
            @return the parse context for parsing the input contained in the given container.
         */
        parse_context(const Input& container)
            : begin(container.begin())
            , end(container.end())
            , position(container.begin())
            , furthest_position(container.begin())
            , m_left_recursion_position(container.begin())
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
        Output remaining_input() const
        {
            return Output(static_cast<Output::const_iterator>(position), static_cast<Output::const_iterator>(end));
        }

        /**
            Helper function for adding a match.
            @param tag match tag.
            @param begin start of matched input.
            @param end end of matched input.
         */
        void add_match(
            const Tag& tag,
            const typename Input::const_iterator begin, 
            const typename Input::const_iterator end)
        {
            matches.push_back(match{ tag, begin, end });
        }

    private:
        //used for identifying recursion
        std::map<const rule<parse_context>*, std::vector<typename Input::const_iterator>> m_rule_positions;

        //left recursion state
        enum class left_recursion_state
        {
            inactive, //not in left recursion
            reject,   //reject left recursive rules, trying to find something to parse
            accept    //accept left recursive rules in order to have some progress
        } m_left_recursion_state = left_recursion_state::inactive;

        //left recursion position; used for identifying progress under left recursion
        typename Input::const_iterator m_left_recursion_position;

        //add rule position
        bool add_position(const rule<parse_context>* rule)
        {
            auto &pos = m_rule_positions[rule];
            pos.push_back(position);
            return pos.size() >= 2 && pos[pos.size() - 1] == pos[pos.size() - 2];
        }

        //remove rule position.
        void remove_position(const rule<parse_context>* rule)
        {
            m_rule_positions[rule].pop_back();
        }

        template <typename L, typename R> friend class choice;
        template <typename T> friend class choice_list;
        friend class rule<parse_context>;
    };


} //namespace parserlib


#endif //PARSERLIB__PARSE_CONTEXT__HPP
