#ifndef PARSERLIB_PARSE_CONTEXT_INTERFACE_HPP
#define PARSERLIB_PARSE_CONTEXT_INTERFACE_HPP


#include <vector>
#include <functional>


namespace parserlib {


    class parse_context_interface;


    using parse_function_type = std::function<bool(parse_context_interface& pc)>;


    class parse_context_interface {
    public:
        virtual bool is_parse_valid() const = 0;

        virtual bool is_parse_end() const = 0;

        virtual bool parse_symbol(int symbol) = 0;

        virtual bool parse_string(const std::vector<int>& string) = 0;

        virtual bool parse_set(const std::vector<int>& set) = 0;

        virtual bool parse_range(int min, int max) = 0;

        virtual bool parse_newline(int symbol) = 0;

        virtual bool parse_newline(const std::vector<int>& string) = 0;

        virtual bool parse_any() = 0;

        virtual bool skip_before(const parse_function_type& fn) = 0;

        virtual bool skip_after(const parse_function_type& fn) = 0;

        virtual bool parse(const parse_function_type& fn) = 0;

        virtual bool test_parse(const parse_function_type& fn) = 0;

        virtual void push_match_start_state() = 0;

        virtual void pop_match_start_state() = 0;

        virtual void add_match(int match_id) = 0;

        virtual void push_error_start_state() = 0;

        virtual void pop_error_start_state() = 0;

        virtual void add_error(int error_id) = 0;

        virtual bool parse_loop(const parse_function_type& fn) = 0;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_CONTEXT_INTERFACE_HPP
