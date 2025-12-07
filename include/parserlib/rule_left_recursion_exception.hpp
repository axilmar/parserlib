#ifndef PARSERLIB_RULE_LEFT_RECURSION_EXCEPTION_HPP
#define PARSERLIB_RULE_LEFT_RECURSION_EXCEPTION_HPP


namespace parserlib {


    template <class ParseContext> class rule;


    template <class ParseContext>
    class rule_left_recursion_exception {
    public:
        using rule_type = rule<ParseContext>;

        rule_left_recursion_exception(rule_type& rule)
            : m_rule(rule)
        {
        }

        rule_type& get_rule() const {
            return m_rule;
        }

    private:
        rule_type& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_LEFT_RECURSION_EXCEPTION_HPP
