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
template <class T> auto terminal(T&& t) { return pe::terminal(std::forward<T>(t)); }
template <class T> auto one_of(T&& t) { return pe::one_of(std::forward<T>(t)); }
static auto& range = pe::range;
using rule = pe::rule;


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


static void test_ast() {
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

    std::function<double(const ast_node_ptr_type&)> evaluate;

    evaluate = [&](const ast_node_ptr_type& node) {
        switch (node->get_id()) {
            case Number: {
                stringstream stream;
                stream << node->get_source();
                double r;
                stream >> r;
                return r;
            }

            case Add:
                return evaluate(node->get_children()[0]) + evaluate(node->get_children()[1]);

            case Sub:
                return evaluate(node->get_children()[0]) - evaluate(node->get_children()[1]);

            case Mul:
                return evaluate(node->get_children()[0]) * evaluate(node->get_children()[1]);

            case Div:
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
}


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
    test_ast();
    test_calculator();
    //test_performance();
}

