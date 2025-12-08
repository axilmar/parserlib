#ifndef PARSERLIB_INTERFACE_PARSE_CONTEXT_HPP
#define PARSERLIB_INTERFACE_PARSE_CONTEXT_HPP


namespace parserlib {


    class rule;


} //namespace parserlib


namespace parserlib::interface {


    class parse_context {
    public:
        virtual ~parse_context() {
        }

        // parse position API

        virtual bool is_valid_parse_position() const = 0;

        virtual bool is_end_parse_position() const = 0;

        virtual void increment_parse_position() = 0;

        // symbol API

        virtual int get_current_symbol() const = 0;

        virtual int compare_symbols(int a, int b) const = 0;

        // state API

        virtual void push_state() = 0;

        virtual void pop_state() = 0;

        // match API

        virtual void push_match_start_state() = 0;

        virtual void pop_match_start_state() = 0;

        virtual void add_match(int id) = 0;

        // error API
        
        virtual void push_error_start_parse_position() = 0;

        virtual void pop_error_start_parse_position() = 0;

        virtual void add_error(int id) = 0;

        // rule API

        enum class rule_status {
            none,
            reject_left_recursion,
            accept_left_recursion
        };

        virtual bool is_rule_left_recursive_at_current_parse_position(rule& r) const = 0;

        virtual void push_rule_state(rule& r, rule_status rs) = 0;

        virtual void pop_rule_state(rule& r) = 0;
    };


} //namespace parserlib::interface;


#endif //PARSERLIB_INTERFACE_PARSE_CONTEXT_HPP
