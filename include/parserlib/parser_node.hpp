#ifndef PARSERLIB_PARSER_NODE_HPP
#define PARSERLIB_PARSER_NODE_HPP


#include <type_traits>


namespace parserlib {


    template <class Parser> class loop_0_or_more_times_parser_node;
    template <class Parser> class loop_1_or_more_times_parser_node;
    template <class Parser> class optional_parser_node;
    template <class Parser> class logical_and_parser_node;
    template <class Parser> class logical_not_parser_node;


    struct parser_node_tag {
    };


    struct rule_tag {
    };


    template <class Parser> class parser_node : public parser_node_tag {
    public:
        loop_0_or_more_times_parser_node<Parser> operator *() const;

        loop_1_or_more_times_parser_node<Parser> operator +() const;

        optional_parser_node<Parser> operator -() const;

        logical_and_parser_node<Parser> operator &() const;

        logical_not_parser_node<Parser> operator !() const;

        const Parser* this_() const {
            return static_cast<const Parser*>(this);
        }

        Parser* this_() {
            return static_cast<Parser*>(this);
        }
    };


    template <class Parser, std::enable_if_t<std::is_base_of_v<parser_node_tag, Parser> && !std::is_base_of_v<rule_tag, Parser>, bool> = true>
    const Parser& parser(const Parser& parser) {
        return parser;
    }


} //namespace parserlib


#endif // PARSERLIB_PARSER_NODE_HPP
