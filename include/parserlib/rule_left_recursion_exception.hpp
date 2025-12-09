#ifndef PARSERLIB_RULE_LEFT_RECURSION_EXCEPTION_HPP
#define PARSERLIB_RULE_LEFT_RECURSION_EXCEPTION_HPP


namespace parserlib {


    class rule;


    class rule_left_recursion_exception {
    public:
        rule_left_recursion_exception(rule& r) : m_rule(r) {
        }

        rule& get_rule() const {
            return m_rule;
        }

    private:
        rule& m_rule;
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_LEFT_RECURSION_EXCEPTION_HPP
