#include <string>
#include <sstream>
#include "parserlib/parse_context.hpp"
#include "parserlib/terminal_parse_nodes.hpp"
#include "parserlib/unary_parse_nodes.hpp"
#include "parserlib/binary_parse_nodes.hpp"
#include "parserlib/rule.hpp"


using namespace parserlib;


static void test_constructor() {
    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        rule<pc_type> rule1;
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        rule<pc_type> rule1 = 'a';
        std::string source = "a";
        pc_type pc(source);
        assert(rule1.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        rule<pc_type> rule1;
        //rule<pc_type> rule2(rule1);
    }
}


static void test_assignment_operator() {
    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        rule<pc_type> rule1;
        rule1 = 'a';
        std::string source = "a";
        pc_type pc(source);
        assert(rule1.parse(pc));
        assert(pc.is_end_parse_position());
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        rule<pc_type> rule1;
        rule<pc_type> rule2;
        //rule = rule2;
    }
}


template <class ParseContext>
struct calculator_grammar {
    enum match_id {
        NUM,
        ADD,
        SUB,
        MUL,
        DIV
    };

    rule<ParseContext> mul;
    rule<ParseContext> add;

    calculator_grammar() {
        const auto dgt = range('0', '9');
        const auto dec = +dgt;
        const auto num = (dec >> -('.' >> dec))->*NUM;

        const auto val
            = num
            | '(' >> add >> ')';

        mul = (mul >> '*' >> val)->*MUL
            | (mul >> '/' >> val)->*DIV
            | val;

        add = (add >> '+' >> mul)->*ADD
            | (add >> '-' >> mul)->*SUB
            | mul;
    }

    template <class ParseContext>
    parse_result parse(ParseContext& pc) noexcept {
        return add.parse(pc);
    }

    static double eval(const typename ParseContext::match& m) {
        switch (m.id()) {
            case NUM: {
                std::stringstream stream;
                stream << m.source();
                double v;
                stream >> v;
                return v;
            }

            case ADD:
                assert(m.children().size() == 2);
                return eval(m.children()[0]) + eval(m.children()[1]);

            case SUB:
                assert(m.children().size() == 2);
                return eval(m.children()[0]) - eval(m.children()[1]);

            case MUL:
                assert(m.children().size() == 2);
                return eval(m.children()[0]) * eval(m.children()[1]);

            case DIV:
                assert(m.children().size() == 2);
                return eval(m.children()[0]) / eval(m.children()[1]);
        }

        throw std::invalid_argument("match");
    }
};


static void test_left_recursion() {
    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1+2";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0+2.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1+2*3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 + 2.0 * 3.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1+2/3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 + 2.0 / 3.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1-2*3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 - 2.0 * 3.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1-2/3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 - 2.0 / 3.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1*2+3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 * 2.0 + 3.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1*2-3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 * 2.0 - 3.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1/2+3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 / 2.0 + 3.0);
    }

    {
        using pc_type = parse_context<std::string, int, int, case_sensitive_comparator>;
        calculator_grammar<pc_type> grammar;
        std::string source = "1/2-3";
        pc_type pc(source);
        assert(grammar.parse(pc));
        assert(pc.matches().size() == 1);
        assert(calculator_grammar<pc_type>::eval(pc.matches()[0]) == 1.0 / 2.0 - 3.0);
    }
}


void test_rule() {
    test_constructor();
    test_assignment_operator();
    test_left_recursion();
}
