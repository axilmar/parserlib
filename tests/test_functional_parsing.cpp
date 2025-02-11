#include <cassert>
#include <string>
#include <sstream>
#include <stdexcept>
#include "parserlib/functional_parsing.hpp"


using namespace parserlib::functional;


struct test_common {
    using SourceType = std::string;
    using MatchIdType = int;
    using TerminalTraitsType = parse_terminal_traits;
    using context = parse_context<SourceType, MatchIdType, TerminalTraitsType>;
    using state = typename context::state;
    using parse_result = parserlib::functional::parse_result<SourceType, MatchIdType>;
};


struct test_parse_terminal_value : test_common {
    static void test() {
        SourceType source = "a";
        {
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, 'a'); });
            assert(result.success);
            assert(result.parse_position == source.end());
        }
        {
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, 'b'); });
            assert(!result.success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_terminal_string : test_common {
    static void test() {
        SourceType source = "abc";
        {
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, "abc"); });
            assert(result.success);
            assert(result.parse_position == source.end());
        }
        {
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, "abd"); });
            assert(!result.success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_terminal_set : test_common {
    static void test() {
        const std::set<char> values({ 'a', 'b', 'c' });
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, values); });
            assert(result.success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "b";
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, values); });
            assert(result.success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "c";
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, values); });
            assert(result.success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "d";
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, values); });
            assert(!result.success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_terminal_range : test_common {
    static void test() {
        for(char digit = '0'; digit <= '9'; ++digit)
        {
            std::stringstream stream;
            stream << digit;
            SourceType source = stream.str();
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, '0', '9'); });
            assert(result.success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, [&](context& c, const state& s) { return parse_terminal(c, '0', '9'); });
            assert(!result.success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_logical_and : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_logical_and(c, s, [](context&c, const state& s) {
            return parse_terminal(c, 'a');
            });
    }

    static void test() {
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(result.parse_success);
            assert(result.parse_position == source.begin());
        }
        {
            SourceType source = "b";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.parse_success);
            assert(!result.success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_logical_not : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_logical_not(c, s, [](context& c, const state& s) {
            return parse_terminal(c, 'b');
            });
    }

    static void test() {
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(result.parse_success);
            assert(result.parse_position == source.begin());
        }
        {
            SourceType source = "b";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(!result.parse_success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_zero_or_more_times : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_zero_or_more_times(c, s, [](context& c, const state& s) {
            return parse_terminal(c, 'a');
            });
    }

    static void test() {
        {
            SourceType source = "";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.begin());
        }
        {
            SourceType source = "b";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.begin());
        }
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "aa";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "aaa";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "aaab";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.end() - 1);
        }
    }
};


struct test_parse_one_or_more_times : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_one_or_more_times(c, s, [](context& c, const state& s) {
            return parse_terminal(c, 'a');
            });
    }

    static void test() {
        {
            SourceType source = "";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(!result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.begin());
        }
        {
            SourceType source = "b";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(!result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.begin());
        }
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "aa";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "aaa";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "aaab";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.end() - 1);
        }
    }
};


struct test_parse_optional : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_optional(c, s, [](context& c, const state& s) {
            return parse_terminal(c, 'a');
            });
    }

    static void test() {
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "b";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_sequence : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_sequence(c, s, 
            [](context& c, const state& s) { return parse_terminal(c, 'a'); },
            [](context& c, const state& s) { return parse_terminal(c, 'b'); },
            [](context& c, const state& s) { return parse_terminal(c, 'c'); });
    }

    static void test() {
        {
            SourceType source = "abc";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "abd";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(!result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_choice : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_choice(c, s,
            [](context& c, const state& s) { return parse_terminal(c, 'a'); },
            [](context& c, const state& s) { return parse_terminal(c, 'b'); },
            [](context& c, const state& s) { return parse_terminal(c, 'c'); });
    }

    static void test() {
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "b";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "c";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());
        }
        {
            SourceType source = "d";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(!result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_match : test_common {
    static bool grammar(context& c, const state& s) {
        return parse_one_or_more_times(c, s, [](context& c, const state& s) {
            return parse_choice(c, s,
                [](context& c, const state& s) { return parse_match(c, s, 'A', [](context& c, const state& s) { return parse_terminal(c, 'a'); }); },
                [](context& c, const state& s) { return parse_match(c, s, 'B', [](context& c, const state& s) { return parse_terminal(c, 'b'); }); },
                [](context& c, const state& s) { return parse_match(c, s, 'C', [](context& c, const state& s) { return parse_terminal(c, 'c'); }); });
            });
    }

    static void test() {
        {
            SourceType source = "a";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(result.matches[0].id == 'A');
            assert(result.matches[0].start_position == source.begin());
            assert(result.matches[0].end_position == result.matches[0].start_position + 1);
        }
        {
            SourceType source = "cba";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 3);

            assert(result.matches[0].id == 'C');
            assert(result.matches[0].start_position == source.begin());
            assert(result.matches[0].end_position == result.matches[0].start_position + 1);

            assert(result.matches[1].id == 'B');
            assert(result.matches[1].start_position == source.begin() + 1);
            assert(result.matches[1].end_position == result.matches[1].start_position + 1);

            assert(result.matches[2].id == 'A');
            assert(result.matches[2].start_position == source.begin() + 2);
            assert(result.matches[2].end_position == result.matches[2].start_position + 1);
        }
        {
            SourceType source = "d";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(!result.success);
            assert(!result.parse_success);
            assert(!result.completion_success);
            assert(result.parse_position == source.begin());
        }
    }
};


struct test_parse_left_recursion : test_common {
    enum TokenId {
        NUM,
        ADD,
        SUB,
        MUL,
        DIV
    };

    static bool parse_plus(context& c, const state& s) {
        return parse_terminal(c, '+');
    }

    static bool parse_minus(context& c, const state& s) {
        return parse_terminal(c, '-');
    }

    static bool parse_star(context& c, const state& s) {
        return parse_terminal(c, '*');
    }

    static bool parse_slash(context& c, const state& s) {
        return parse_terminal(c, '/');
    }

    static bool parse_left_paren(context& c, const state& s) {
        return parse_terminal(c, '(');
    }

    static bool parse_right_paren(context& c, const state& s) {
        return parse_terminal(c, ')');
    }

    static bool parse_digit(context& c, const state& s) {
        return parse_terminal(c, '0', '9');
    }

    static bool parse_int(context& c, const state& s) {
        return parse_match(c, s, NUM, [](context& c, const state& s) { return parse_one_or_more_times(c, s, parse_digit); });
    }

    static bool parse_num(context& c, const state& s) {
        return parse_choice(c, s,
            parse_int,
            [](context& c, const state& s) { return parse_sequence(c, s, &parse_left_paren, &parse_add, &parse_right_paren); });
    }

    static bool parse_mul(context& c, const state& s) {
        return parse_left_recursion(c, s, &parse_num,
            [](context& c, const state& s) { return parse_match(c, s, MUL, [](context& c, const state& s) { return parse_sequence(c, s, &parse_star, &parse_num); }); },
            [](context& c, const state& s) { return parse_match(c, s, DIV, [](context& c, const state& s) { return parse_sequence(c, s, &parse_slash, &parse_num); }); });
    }

    static bool parse_add(context& c, const state& s) {
        return parse_left_recursion(c, s, &parse_mul, 
            [](context& c, const state& s) { return parse_match(c, s, ADD, [](context& c, const state& s) { return parse_sequence(c, s, &parse_plus, &parse_mul); }); },
            [](context& c, const state& s) { return parse_match(c, s, SUB, [](context& c, const state& s) { return parse_sequence(c, s, &parse_minus, &parse_mul); }); });
    }

    static bool grammar(context& c, const state& s) {
        return parse_add(c, s);
    }

    static double eval(const match<SourceType, MatchIdType>& m) {
        switch (m.id) {
            case NUM: {
                std::stringstream stream;
                stream << m.get_source();
                double v;
                stream >> v;
                return v;
            }

            case ADD:
                return eval(m.children[0]) + eval(m.children[1]);

            case SUB:
                return eval(m.children[0]) - eval(m.children[1]);

            case MUL:
                return eval(m.children[0]) * eval(m.children[1]);

            case DIV:
                return eval(m.children[0]) / eval(m.children[1]);
        }

        throw std::logic_error("Unknown match id.");
    }

    static void test() {
        {
            SourceType source = "1";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == 1);

            assert(result.matches[0].id == NUM);
            assert(result.matches[0].get_source() == "1");
        }
        {
            SourceType source = "1+2";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == 1+2);

            assert(result.matches[0].id == ADD);
            assert(result.matches[0].get_source() == "1+2");

            assert(result.matches[0].children[0].id == NUM);
            assert(result.matches[0].children[0].get_source() == "1");

            assert(result.matches[0].children[1].id == NUM);
            assert(result.matches[0].children[1].get_source() == "2");
        }
        {
            SourceType source = "1+2-3";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == 1 + 2 - 3);
        }
        {
            SourceType source = "(1+2)-3";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == (1 + 2) - 3);
        }
        {
            SourceType source = "1+(2-3)";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == 1 + (2 - 3));
        }
        {
            SourceType source = "1+2-3*4";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == 1 + 2 - 3 * 4);
        }
        {
            SourceType source = "((1+2)-3)*4";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == ((1 + 2) - 3) * 4);
        }
        {
            SourceType source = "(1+(2-3))*4";
            parse_result result = parse<MatchIdType>(source, &grammar);
            assert(result.success);
            assert(result.parse_success);
            assert(result.completion_success);
            assert(result.parse_position == source.end());

            assert(result.matches.size() == 1);

            assert(eval(result.matches[0]) == (1 + (2 - 3)) * 4);
        }
    }
};


void test_functional_parsing() {
    test_parse_terminal_value::test();
    test_parse_terminal_string::test();
    test_parse_terminal_set::test();
    test_parse_terminal_range::test();
    test_parse_logical_and::test();
    test_parse_logical_not::test();
    test_parse_one_or_more_times::test();
    test_parse_optional::test();
    test_parse_sequence::test();
    test_parse_choice::test();
    test_parse_match::test();
    test_parse_left_recursion::test();
}
