#include <sstream>
#include <cmath>
#include "parserlib.hpp"


using namespace parserlib;


static void test_choice_parser() {
    {
        const auto grammar = terminal('a') | terminal('b');
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') | terminal('b');
        std::string source = "b";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') | terminal('b');
        std::string source = "c";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_end_parser() {
    const auto grammar = parserlib::end();
    std::string source = "";
    parse_context<> context(source);
    assert(grammar.parse(context));
    assert(context.is_end_parse_position());
}


static void test_error_parser() {
    {
        const auto grammar = on_error_continue_after(terminal('a') >> ';', 1, ';');
        std::string source = "b;";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = on_error_continue_after(terminal('a') >> terminal(";;"), 1, ";;");
        std::string source = "b;;";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }
}


static void test_logical_and_parser() {
    {
        const auto grammar = &terminal('a');
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }

    {
        const auto grammar = &terminal('a');
        std::string source = "b";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_logical_not_parser() {
    {
        const auto grammar = !terminal('a');
        std::string source = "a";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }

    {
        const auto grammar = !terminal('a');
        std::string source = "b";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_match_parser() {
    {
        const auto grammar = terminal('a') ->* 1;
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(context.matches()[0].token() == 1);
        assert(context.matches()[0].span().begin() == source.begin());
        assert(context.matches()[0].span().end() == source.end());
    }

    {
        const auto grammar = terminal('a') ->* 1;
        std::string source = "b";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
        assert(context.matches().size() == 0);
    }
}


static void test_one_or_more_parser() {
    {
        const auto grammar = +terminal('a');
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = +terminal('a');
        std::string source = "aa";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = +terminal('a');
        std::string source = "aab";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position() - 1);
    }

    {
        const auto grammar = +terminal('a');
        std::string source = "ba";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.is_end_parse_position() - 1);
    }
}


static void test_optional_parser() {
    {
        const auto grammar = -terminal("a");
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = -terminal("a");
        std::string source = "";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = -terminal("a");
        std::string source = "b";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


struct calculator {
    enum OutputTokenId {
        NUM,
        ADD,
        SUB,
        MUL,
        DIV
    };

    rule<> mul;
    rule<> add;
    rule<> grammar;

    calculator() {
        auto digit = range('0', '9');

        auto num = (+digit >> -('.' >> +digit)) ->* NUM;

        auto val = num
                 | '(' >> add >> ')';

        mul = (mul >> '*' >> val) ->* MUL
            | (mul >> '/' >> val) ->* DIV
            | val;

        add = (add >> '+' >> mul) ->* ADD
            | (add >> '-' >> mul) ->* SUB
            | mul;

        grammar = add;
    }

    template <class Match>
    double evaluate(const Match& match) const noexcept {
        switch (match.token()) {
            case NUM:
            {
                assert(match.children().size() == 0);
                std::stringstream stream;
                stream << match.source();
                double result{};
                stream >> result;
                return result;
            }

            case ADD:
            {
                assert(match.children().size() == 2);
                const double result = evaluate(match.children()[0]) + evaluate(match.children()[1]);
                return result;
            }

            case SUB:
            {
                assert(match.children().size() == 2);
                const double result = evaluate(match.children()[0]) - evaluate(match.children()[1]);
                return result;
            }

            case MUL:
            {
                assert(match.children().size() == 2);
                const double result = evaluate(match.children()[0]) * evaluate(match.children()[1]);
                return result;
            }

            case DIV:
            {
                assert(match.children().size() == 2);
                const double divisor = evaluate(match.children()[1]);
                const double result = fpclassify(divisor) != FP_ZERO ? (evaluate(match.children()[0]) / divisor) : 0;
                return result;
            }
        }

        assert(false);
        return 0;
    }
};


static void test_rule() {
    {
        rule<> grammar = 'a';
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        rule<> grammar = 'a';
        std::string source = "b";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }

    {
        rule<> grammar = 'a' >> -grammar;
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        rule<> grammar = 'a' >> -grammar;
        std::string source = "aa";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        calculator calc;
        std::string source = "1";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 1);
    }

    {
        calculator calc;
        std::string source = "2.0/1.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0/1.0);
    }

    {
        calculator calc;
        std::string source = "2.0*1.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0);
    }

    {
        calculator calc;
        std::string source = "2.0-1.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0);
    }

    {
        calculator calc;
        std::string source = "2.0+1.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0);
    }

    {
        calculator calc;
        std::string source = "2.0/1.0*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 / 1.0 * 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0/1.0)*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 / 1.0) * 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0/(1.0*3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 / (1.0 * 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0*1.0/3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0 / 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0*1.0)/3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 * 1.0) / 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0*(1.0/3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * (1.0 / 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0/1.0-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 / 1.0 - 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0/1.0)-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 / 1.0) - 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0/(1.0-3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 / (1.0 - 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0-1.0/3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0 / 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0-1.0)/3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 - 1.0) / 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0-(1.0/3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - (1.0 / 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0/1.0+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 / 1.0 + 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0/1.0)+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 / 1.0) + 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0/(1.0+3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 / (1.0 + 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0+1.0/3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 / 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0+1.0)/3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 + 1.0) / 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0+(1.0/3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + (1.0 / 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0*1.0*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0 * 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0*1.0)*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 * 1.0) * 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0*(1.0*3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * (1.0 * 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0*1.0*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0 * 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0*1.0)*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 * 1.0) * 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0*(1.0*3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * (1.0 * 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0*1.0-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0 - 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0*1.0)-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 * 1.0) - 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0*(1.0-3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * (1.0 - 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0-1.0*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0 * 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0-1.0)*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 - 1.0) * 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0-(1.0*3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - (1.0 * 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0*1.0+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0 + 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0*1.0)+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 * 1.0) + 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0*(1.0+3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * (1.0 + 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0+1.0*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 * 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0+1.0)*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 + 1.0) * 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0+(1.0*3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + (1.0 * 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0-1.0*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0 * 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0-1.0)*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 - 1.0) * 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0-(1.0*3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - (1.0 * 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0*1.0-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0 - 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0*1.0)-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 * 1.0) - 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0*(1.0-3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * (1.0 - 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0-1.0-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0 - 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0-1.0)-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 - 1.0) - 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0-(1.0-3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - (1.0 - 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0-1.0-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0 - 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0-1.0)-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 - 1.0) - 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0-(1.0-3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - (1.0 - 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0-1.0+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0 + 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0-1.0)+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 - 1.0) + 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0-(1.0+3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - (1.0 + 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0+1.0-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 - 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0+1.0)-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 + 1.0) - 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0+(1.0-3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + (1.0 - 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0+1.0*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 * 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0+1.0)*3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 + 1.0) * 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0+(1.0*3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + (1.0 * 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0*1.0+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * 1.0 + 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0*1.0)+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 * 1.0) + 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0*(1.0+3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 * (1.0 + 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0+1.0-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 - 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0+1.0)-3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 + 1.0) - 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0+(1.0-3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + (1.0 - 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0-1.0+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - 1.0 + 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0-1.0)+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 - 1.0) + 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0-(1.0+3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 - (1.0 + 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0+1.0+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 + 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0+1.0)+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 + 1.0) + 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0+(1.0+3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + (1.0 + 3.0));
    }

    {
        calculator calc;
        std::string source = "2.0+1.0+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + 1.0 + 3.0);
    }

    {
        calculator calc;
        std::string source = "(2.0+1.0)+3.0";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == (2.0 + 1.0) + 3.0);
    }

    {
        calculator calc;
        std::string source = "2.0+(1.0+3.0)";
        parse_context<> context(source);
        assert(calc.grammar.parse(context));
        assert(context.is_end_parse_position());
        assert(context.matches().size() == 1);
        assert(calc.evaluate(context.matches()[0]) == 2.0 + (1.0 + 3.0));
    }
}


static void test_sequence_parser() {
    {
        const auto grammar = terminal('a') >> 'b' >> 'c' >> 'd';
        std::string source = "abcd";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = (terminal('a') >> 'b') >> 'c' >> 'd';
        std::string source = "abcd";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> 'b' >> (terminal('c') >> 'd');
        std::string source = "abcd";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = (terminal('a') >> 'b') >> (terminal('c') >> 'd');
        std::string source = "abcd";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a') >> 'b' >> 'c' >> 'd';
        std::string source = "abce";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_terminal_parser() {
    {
        const auto grammar = terminal('a');
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = terminal('a');
        std::string source = "b";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_terminal_string_parser() {
    {
        const auto grammar = terminal("abc");
        std::string source = "abc";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = terminal("abc");
        std::string source = "abd";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_terminal_range_parser() {
    {
        const auto grammar = range('0', '9');
        std::string source = "1";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = range('0', '9');
        std::string source = "a";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_terminal_set_parser() {
    {
        const auto grammar = one_of({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        std::string source = "1";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = one_of({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' });
        std::string source = "a";
        parse_context<> context(source);
        assert(!grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


static void test_zero_or_more_parser() {
    {
        const auto grammar = *terminal('a');
        std::string source = "a";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = *terminal('a');
        std::string source = "aa";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position());
    }

    {
        const auto grammar = *terminal('a');
        std::string source = "aab";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.is_end_parse_position() - 1);
    }

    {
        const auto grammar = *terminal('a');
        std::string source = "ba";
        parse_context<> context(source);
        assert(grammar.parse(context));
        assert(context.parse_position() == source.begin());
    }
}


void test_parser_classes() {
    test_choice_parser();
    test_end_parser();
    test_error_parser();
    test_logical_and_parser();
    test_logical_not_parser();
    test_match_parser();
    test_one_or_more_parser();
    test_optional_parser();
    test_rule();
    test_sequence_parser();
    test_terminal_parser();
    test_terminal_string_parser();
    test_terminal_range_parser();
    test_terminal_set_parser();
    test_zero_or_more_parser();
}
