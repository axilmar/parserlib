#include <cassert>
#include <sstream>
#include <chrono>
#include <iostream>
#include "parserlib/parser_engine.hpp"


using namespace std;
using namespace parserlib;


using pe = parser_engine<>;
using parse_result = pe::parse_result;
using parse_context = pe::parse_context;
using ast_node_ptr_type = pe::ast_node_ptr_type;
using match = pe::match;
using rule = pe::rule;
using match_container_type = pe::match_container_type;
using ast_node_container_type = pe::ast_node_container_type;
template <class T> auto terminal(T&& t) { return pe::terminal(std::forward<T>(t)); }
template <class T> auto one_of(T&& t) { return pe::one_of(std::forward<T>(t)); }
template <class T> auto range(T&& min, T&& max) { return pe::range(std::forward<T>(min), std::forward<T>(max)); }


static void test_terminal_value_parser() {
    auto grammar = terminal('a');

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_terminal_string_parser() {
    auto grammar = terminal("abc");

    //test success
    {
        string input = "abc";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "aba";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_terminal_set_parser() {
    auto grammar = one_of("0123456789");

    //test success
    {
        string input = "0";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "5";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "9";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_terminal_range_parser() {
    auto grammar = range('0', '9');

    //test success
    {
        string input = "0";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "5";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "9";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_zero_or_more_parser() {
    auto grammar = *terminal('a');

    //test success
    {
        string input = "";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "aaa";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_one_or_more_parser() {
    auto grammar = +terminal('a');

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "aaa";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }

    //test failure
    {
        string input = "";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_optional_parser() {
    auto grammar = -terminal('a');

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success on wrong input
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.get_current_position() == input.begin());
    }

    //test success on wrong input
    {
        string input = "";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_logical_and_parser() {
    auto grammar = &terminal('a');

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.get_current_position() == input.begin());
    }

    //test failure
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_logical_not_parser() {
    auto grammar = !terminal('a');

    //test success
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.get_current_position() == input.begin());
    }

    //test failure
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_sequence_parser() {
    auto grammar = terminal('a') >> 'b' >> 'c';

    //test success
    {
        string input = "abc";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "bca";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }

    //test failure
    {
        string input = "";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_choice_parser() {
    auto grammar = terminal('a') | 'b' | 'c';

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "c";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "d";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }

    //test failure
    {
        string input = "";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_rule() {
    rule grammar = terminal('a');

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "b";
        parse_context pc(input);
        parse_result result = grammar.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }

    //test rule constructor
    {
        rule a = grammar;
        assert(a.get_parser_type() == typeid(pe::rule_reference_parser));
    }

    //test rule assignment
    {
        rule a;
        a = grammar;
        assert(a.get_parser_type() == typeid(pe::rule_reference_parser));
    }
}


static void test_recursion() {
    rule r = terminal('a') >> *r >> terminal('c')
           | 'b';

    //test success
    {
        string input = "ac";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "abc";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "abbc";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "abbbc";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "1bc";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }

    //test failure
    {
        string input = "a1c";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }

    //test failure
    {
        string input = "ab1";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }
}


static void test_left_recursion() {
    rule r = r >> 'b'
          | 'a';

    //test success
    {
        string input = "a";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test success
    {
        string input = "ab";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::success);
        assert(pc.is_end_position());
    }

    //test failure
    {
        string input = "1b";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::failure);
        assert(pc.get_current_position() == input.begin());
    }

    //test partial success
    {
        string input = "a1";
        parse_context pc(input);
        parse_result result = r.parse(pc);
        assert(result == parse_result::success);
        assert(pc.get_current_position() == std::next(input.begin()));
    }
}


static void test_matches() {
    enum MatchId {
        A,
        B,
        C
    };

    auto a = +terminal('a') ->* A;
    auto b = +terminal('b') ->* B;
    auto c = +terminal('c') ->* C;
    auto grammar = *(a | b | c | ' ');

    string input = "aaa bbb ccc aaa ccc bbb";
    parse_context pc(input);
    grammar.parse(pc);
    const auto& matches = pc.get_matches();

    assert(matches.size() == 6);

    for (size_t i = 0; i < matches.size(); ++i) {
        const size_t index = i * 4;
        assert(matches[i].get_start_position() == std::next(input.begin(), index));
        assert(matches[i].get_end_position() == std::next(input.begin(), index + 3));
        assert(matches[i].get_source() == input.substr(index, 3));
    }
}


static void test_left_recursion_matches() {
    enum MatchId {
        Number,
        Add,
        Sub,
        Mul,
        Div
    };

    auto digit = range('0', '9');

    auto number = +digit ->* Number;

    auto val = number;

    rule mul = (mul >> '*' >> val) ->* Mul
             | (mul >> '/' >> val) ->* Div
             | val;

    rule add = (add >> '+' >> mul) ->* Add
             | (add >> '-' >> mul) ->* Sub
             | mul;

    auto grammar = add;

    //min to max operator precedence
    {
        string input = "1+2-3*4/5";
        parse_context pc(input);
        grammar.parse(pc);
        const auto& matches = pc.get_matches();

        assert(matches.size() == 1);

        for (const auto& match : matches) {
            switch (match.get_id()) {
                case Add:
                    assert(match.get_source() == "1+2");
                    break;

                case Sub:
                    assert(match.get_source() == "1+2-3*4/5");
                    break;

                case Mul:
                    assert(match.get_source() == "3*4");
                    break;

                case Div:
                    assert(match.get_source() == "3*4/5");
                    break;
            }
        }
    }

    //max to min operator precedence
    {
        string input = "1/2*3-4+5";
        parse_context pc(input);
        grammar.parse(pc);
        const auto& matches = pc.get_matches();

        assert(matches.size() == 1);

        for (const auto& match : matches) {
            switch (match.get_id()) {
            case Add:
                assert(match.get_source() == "1/2*3-4+5");
                break;

            case Sub:
                assert(match.get_source() == "1/2*3-4");
                break;

            case Mul:
                assert(match.get_source() == "1/2*3");
                break;

            case Div:
                assert(match.get_source() == "1/2");
                break;
            }
        }
    }

    //min to max operator precedence inside rule, max to min operator precedence outside rule
    {
        string input = "1*2/3+4-5";
        parse_context pc(input);
        grammar.parse(pc);
        const auto& matches = pc.get_matches();

        assert(matches.size() == 1);

        for (const auto& match : matches) {
            switch (match.get_id()) {
            case Add:
                assert(match.get_source() == "1*2/3+4");
                break;

            case Sub:
                assert(match.get_source() == "1*2/3+4-5");
                break;

            case Mul:
                assert(match.get_source() == "1*2");
                break;

            case Div:
                assert(match.get_source() == "1*2/3");
                break;
            }
        }
    }

    //max to min operator precedence inside rule, min to max operator precedence outside rule
    {
        string input = "1-2+3/4*5";
        parse_context pc(input);
        grammar.parse(pc);
        const auto& matches = pc.get_matches();

        assert(matches.size() == 1);

        for (const auto& match : matches) {
            switch (match.get_id()) {
            case Add:
                assert(match.get_source() == "1-2+3/4*5");
                break;

            case Sub:
                assert(match.get_source() == "1-2");
                break;

            case Mul:
                assert(match.get_source() == "3/4*5");
                break;

            case Div:
                assert(match.get_source() == "3/4");
                break;
            }
        }
    }
}


namespace test_ast {


    enum class MatchId {
        Number,
        Add,
        Sub,
        Mul,
        Div
    };


    template <typename Elem, typename Traits>
    std::basic_ostream<Elem, Traits>& operator << (std::basic_ostream<Elem, Traits>& stream, MatchId id) {
        switch (id) {
        case MatchId::Number:
            stream << "Number";
            break;

        case MatchId::Add:
            stream << "Add";
            break;

        case MatchId::Sub:
            stream << "Sub";
            break;

        case MatchId::Mul:
            stream << "Mul";
            break;

        case MatchId::Div:
            stream << "Div";
            break;

        }
        return stream;
    }


    static void test_ast() {
        using pe = parser_engine<std::string, MatchId>;

        auto digit = pe::range('0', '9');

        auto number = +digit->*MatchId::Number;

        auto val = number;

        pe::rule mul = (mul >> '*' >> val)->*MatchId::Mul
            | (mul >> '/' >> val)->*MatchId::Div
            | val;

        pe::rule add = (add >> '+' >> mul)->*MatchId::Add
            | (add >> '-' >> mul)->*MatchId::Sub
            | mul;

        auto grammar = add;

        std::function<double(const pe::ast_node_ptr_type&)> evaluate;

        evaluate = [&](const pe::ast_node_ptr_type& node) {
            switch (node->get_id()) {
            case MatchId::Number: {
                stringstream stream;
                stream << node->get_source();
                double r;
                stream >> r;
                return r;
            }

            case MatchId::Add:
                return evaluate(node->get_children()[0]) + evaluate(node->get_children()[1]);

            case MatchId::Sub:
                return evaluate(node->get_children()[0]) - evaluate(node->get_children()[1]);

            case MatchId::Mul:
                return evaluate(node->get_children()[0]) * evaluate(node->get_children()[1]);

            case MatchId::Div:
                return evaluate(node->get_children()[0]) / evaluate(node->get_children()[1]);
            }

            throw std::logic_error("Invalid node id type");
            };

        {
            string input = "1";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1.);
        }

        {
            string input = "1+2";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. + 2.);
        }

        {
            string input = "1-2";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. - 2.);
        }

        {
            string input = "1*2";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. * 2.);
        }

        {
            string input = "1/2";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. / 2.);
        }

        {
            string input = "1+2-3";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. + 2. - 3.);
        }

        {
            string input = "1+2-3*4";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. + 2. - 3. * 4.);
        }

        {
            string input = "1+2-3*4/5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. + 2. - 3. * 4. / 5.);
        }

        {
            string input = "1-2+3/4*5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. - 2. + 3. / 4. * 5.);
        }

        {
            string input = "1*2/3+4-5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. * 2. / 3. + 4. - 5.);
        }

        {
            string input = "1/2*3-4+5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. / 2. * 3. - 4. + 5.);
        }

        {
            string input = "1+2+3+4+5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. + 2. + 3. + 4. + 5.);
        }

        {
            string input = "1+2-3+4-5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. + 2. - 3. + 4. - 5.);
        }

        {
            string input = "1-2-3-4-5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. - 2. - 3. - 4. - 5.);
        }

        {
            string input = "1-2+3-4+5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. - 2. + 3. - 4. + 5.);
        }

        {
            string input = "1*2*3*4*5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. * 2. * 3. * 4. * 5.);
        }

        {
            string input = "1/2/3/4/5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. / 2. / 3. / 4. / 5.);
        }

        {
            string input = "1*2/3*4/5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. * 2. / 3. * 4. / 5.);
        }

        {
            string input = "1/2*3/4*5";
            auto [success, ast, it] = pe::parse(input, grammar);
            assert(ast.size() == 1);
            assert(evaluate(ast[0]) == 1. / 2. * 3. / 4. * 5.);
        }

        //{
        //    string input = "1+2-3*4/5";
        //    auto [success, ast, it] = pe::parse(input, grammar);
        //    for (const auto& astn : ast) {
        //        std::cout << astn;
        //    }
        //}
    }


} //namespace test_ast


template <typename SourceT = std::string> class calculator_grammar {
public:
    enum class ast_id {
        number,
        add,
        sub,
        mul,
        div
    };

    using pe = parserlib::parser_engine<SourceT, ast_id>;

    calculator_grammar() {
        auto digit = pe::range('0', '9');

        auto number = (-pe::one_of("+-") >> +digit >> -('.' >> +digit))->*ast_id::number;

        auto val
            = '(' >> m_add >> ')'
            | number;

        m_mul
            = (m_mul >> '*' >> val) ->* ast_id::mul
            | (m_mul >> '/' >> val) ->* ast_id::div
            | val;

        m_add
            = (m_add >> '+' >> m_mul) ->* ast_id::add
            | (m_add >> '-' >> m_mul) ->* ast_id::mul
            | m_mul;
    }

    double evaluate(const SourceT& input) {
        SourceT source = input;
        auto [success, ast, it] = pe::parse(source, m_add);
        if (success && ast.size() == 1) {
            return evaluate_ast(ast[0]);
        }
        throw std::invalid_argument("Parse error: " + std::string(it, source.cend()));
    }

private:
    typename pe::rule m_add, m_mul;

    static double evaluate_ast(const typename pe::ast_node_ptr_type& node) {
        switch (node->get_id()) {
            case ast_id::number: {
                std::stringstream stream;
                stream << node->get_source();
                double r;
                stream >> r;
                return r;
            }

            case ast_id::add:
                return evaluate_ast(node->get_children()[0]) + evaluate_ast(node->get_children()[1]);

            case ast_id::sub:
                return evaluate_ast(node->get_children()[0]) - evaluate_ast(node->get_children()[1]);

            case ast_id::mul:
                return evaluate_ast(node->get_children()[0]) * evaluate_ast(node->get_children()[1]);

            case ast_id::div:
                return evaluate_ast(node->get_children()[0]) / evaluate_ast(node->get_children()[1]);
        }

        throw std::logic_error("invalid ast id");
    }
};


static void test_calculator() {
    calculator_grammar<> calculator;
    double result = calculator.evaluate("1+(5*6)/2");

    assert(result == (1.0 + (5.0 * 6.0) / 2.0));
}


//static void test_performance() {
//    rule alphabet = terminal('a') >> 'b' >> 'c' >> 'd' >> 'e' >> 'f' >> 'g' >> 'h' >> 'i' >> 'j' >> 'k' >> 'l' >> 'm' >> 'n' >> 'o' >> 'p' >> 'q' >> 'r' >> 's' >> 't' >> 'u' >> 'v' >> 'w' >> 'x' >> 'y' >> 'z';
//
//    auto rule1 = alphabet >> '1';
//    auto rule2 = alphabet >> '2';
//    auto rule3 = alphabet >> '3';
//
//    auto grammar = *(rule1 | rule2 | rule3);
//
//    stringstream stream;
//
//    for (size_t i = 0; i < 1000000; ++i) {
//        stream << "abcdefghijklmnopqrstuvwxyz" << (((i % 3) + 1));
//    }
//
//    string input = stream.str();
//
//    parse_context pc(input);
//    auto start_time = std::chrono::high_resolution_clock::now();
//    parse_result result = grammar.parse(pc);
//    auto end_time = std::chrono::high_resolution_clock::now();
//
//    assert(result == parse_result::success && pc.is_end_position());
//
//    auto duration = end_time - start_time;
//    std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(duration).count() << std::endl;
//}


static void test_contextual_tokenization() {
    enum token_type {
        IDENTIFIER,
        LEFT_SHIFT,
        RIGHT_SHIFT,
        LESS_THAN,
        GREATER_THAN,
    };

    auto whitespace = terminal(' ');

    auto letter = pe::range('a', 'z') | pe::range('A', 'Z');
    auto digit = pe::range('0', '9');
    auto identifier = (letter >> *(letter | digit))->*IDENTIFIER;

    auto left_shift = pe::terminal("<<")->*LEFT_SHIFT;
    auto right_shift = pe::terminal(">>")->*RIGHT_SHIFT;

    auto less_than = pe::terminal('<')->*LESS_THAN;
    auto greater_than = pe::terminal('>')->*GREATER_THAN;

    pe::rule generics_specification
        = identifier >> less_than >> -generics_specification >> greater_than
        | identifier;

    const auto token
        = whitespace
        | generics_specification
        | identifier
        | left_shift
        | right_shift
        | less_than
        | greater_than;

    auto grammar = *token;

    string input = ">>><<<foo<bar<cee>>><<>><";

    auto [success, ast, it] = pe::parse(input, grammar);

    assert(ast.size() == 15);

    assert(ast[0]->get_id() == RIGHT_SHIFT);
    assert(ast[1]->get_id() == GREATER_THAN);
    assert(ast[2]->get_id() == LEFT_SHIFT);
    assert(ast[3]->get_id() == LESS_THAN);
    assert(ast[4]->get_id() == IDENTIFIER);
    assert(ast[5]->get_id() == LESS_THAN);
    assert(ast[6]->get_id() == IDENTIFIER);
    assert(ast[7]->get_id() == LESS_THAN);
    assert(ast[8]->get_id() == IDENTIFIER);
    assert(ast[9]->get_id() == GREATER_THAN);
    assert(ast[10]->get_id() == GREATER_THAN);
    assert(ast[11]->get_id() == GREATER_THAN);
    assert(ast[12]->get_id() == LEFT_SHIFT);
    assert(ast[13]->get_id() == RIGHT_SHIFT);
    assert(ast[14]->get_id() == LESS_THAN);
}


static void test_contextual_parsing() {
    /* tokenizer */

    enum TOKEN {
        IDENTIFIER,
        INTEGER,
        STAR,
        ASSIGN,
        SEMICOLON,
        TYPEDEF,
        INT
    };

    auto whitespace = range('\0', ' ');

    auto digit = range('0', '9');
    auto letter = range('a', 'z') | range('A', 'Z');

    auto typedef_ = terminal("typedef") ->* TYPEDEF;

    auto int_ = terminal("int") ->* INT;

    auto identifier = (letter >> *(letter | digit | '_')) ->* IDENTIFIER;

    auto integer   = +digit              ->* INTEGER  ;
    auto star      = terminal('*')       ->* STAR     ;
    auto assign    = terminal('=')       ->* ASSIGN   ;
    auto semicolon = terminal(';')       ->* SEMICOLON;

    auto token
        = whitespace
        | typedef_
        | int_
        | identifier
        | integer
        | star
        | assign
        | semicolon;

    auto tokenizer_grammar = *token;

    /* parser */

    using parser_pe = parser_engine<ast_node_container_type>;

    enum AST {
        TYPE_INT,
        TYPE_NAME,
        TYPE_POINTER,
        DECLARATION_TYPEDEF,
        DECLARATION_VARIABLE,
        EXPRESSION_MULTIPLICATION,
        EXPRESSION_NAME,
        EXPRESSION_INTEGER,
        VAR_NAME
    };

    auto base_type
        = parser_pe::terminal(INT) ->* TYPE_INT
        | parser_pe::terminal(IDENTIFIER) ->* TYPE_NAME;

    auto pointer_type
        = (base_type >> STAR) ->* TYPE_POINTER
        | base_type;

    auto type_expression = pointer_type;

    auto typedef_declaration = (parser_pe::terminal(TYPEDEF) >> type_expression >> IDENTIFIER >> SEMICOLON) ->* DECLARATION_TYPEDEF;

    auto value
        = parser_pe::terminal(INTEGER) ->* EXPRESSION_INTEGER
        | parser_pe::terminal(IDENTIFIER) ->* EXPRESSION_NAME;

    auto multiplication
        = (value >> STAR >> value) ->* EXPRESSION_MULTIPLICATION
        | value;

    auto expression = multiplication;

    auto match_variable_or_multiplication = [](parser_pe::parse_context& pc, parser_pe::match_container_type& matches) {
        //in order for the variable declaration to look like a multiplication,
        //it shall have two members
        if (matches.size() != 2) {
            return DECLARATION_VARIABLE;
        }

        const auto pointerTypeMatch = matches[0];

        //if the first member is not a pointer type, 
        //then the declaration is a variable
        if (pointerTypeMatch.get_id() != TYPE_POINTER) {
            return DECLARATION_VARIABLE;
        }

        const auto typenameMatch = pointerTypeMatch[0];
           
        //if the base type of the pointer type is not a type name,
        //then the declaration is a variable
        if (typenameMatch.get_id() != TYPE_NAME) {
            return DECLARATION_VARIABLE;
        }

        //the type name that might be a variable identifier 
        const auto id = (*typenameMatch.get_start_position())->get_source();

        //for a multiplication to be valid, there must be a previous variable declaration that has an identifier
        //equal to the id found above
        for (const auto& m : pc.get_matches()) {
            if (m.get_id() == DECLARATION_VARIABLE) {
                auto varName = (*m.find_child_by_id(VAR_NAME)->get_start_position())->get_source();
                if (varName == id) {
                    //replace the pointer match with an expression name match
                    matches[0] = parser_pe::match(EXPRESSION_NAME, typenameMatch.get_start_position(), typenameMatch.get_end_position());
                    return EXPRESSION_MULTIPLICATION;
                }
            }
        }

        //did not find a variable with the given name; make the match a variable
        return DECLARATION_VARIABLE;
        };

    auto variable_declaration = (type_expression >> (parser_pe::terminal(IDENTIFIER) ->* VAR_NAME) >> -(ASSIGN >> expression) >> SEMICOLON) ->* match_variable_or_multiplication;

    auto declaration
        = typedef_declaration
        | variable_declaration;

    auto parser_grammar = *declaration;

    {
        string input = 
            "typedef int x;"
            "int y = 0;"
            "x* a;"
            "y* b;"
            ;

        auto [tokenizer_success, tokens, token_it] = pe::parse(input, tokenizer_grammar);

        auto [parser_success, ast, parser_it] = parser_pe::parse(tokens, parser_grammar);

        assert(ast.size() == 4);
        assert(ast[0]->get_id() == DECLARATION_TYPEDEF);
        assert(ast[1]->get_id() == DECLARATION_VARIABLE);
        assert(ast[2]->get_id() == DECLARATION_VARIABLE);
        assert(ast[3]->get_id() == EXPRESSION_MULTIPLICATION);
    }
}


void test_parser_engine() {
    test_terminal_value_parser();
    test_terminal_string_parser();
    test_terminal_set_parser();
    test_terminal_range_parser();
    test_zero_or_more_parser();
    test_one_or_more_parser();
    test_optional_parser();
    test_logical_and_parser();
    test_logical_not_parser();
    test_sequence_parser();
    test_choice_parser();
    test_rule();
    test_recursion();
    test_left_recursion();
    test_matches();
    test_left_recursion_matches();
    test_ast::test_ast();
    test_calculator();
    //test_performance();
    test_contextual_tokenization();
    test_contextual_parsing();
}

