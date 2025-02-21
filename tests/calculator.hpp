#ifndef PARSERLIB_TESTS_CALCULATOR_HPP
#define PARSERLIB_TESTS_CALCULATOR_HPP


#include <sstream>
#include <cmath>
#include "parserlib.hpp"


namespace parserlib::tests {


    struct calculator {
        enum OutputTokenId {
            NUM,
            ADD,
            SUB,
            MUL,
            DIV
        };

        static const char* get_id_name(int id) {
            switch (id) {
                case NUM: return "num";
                case ADD: return "add";
                case SUB: return "sub";
                case MUL: return "mul";
                case DIV: return "div";
            }
            throw std::invalid_argument("id");
        }

        rule<> mul;
        rule<> add;
        rule<> grammar;

        calculator() {
            auto digit = range('0', '9');

            auto num = (+digit >> -('.' >> +digit))->*NUM;

            auto val = num
                | '(' >> add >> ')';

            mul = (mul >> '*' >> val)->*MUL
                | (mul >> '/' >> val)->*DIV
                | val;

            add = (add >> '+' >> mul)->*ADD
                | (add >> '-' >> mul)->*SUB
                | mul;

            grammar = add;
        }

        template <class Match>
        double evaluate(const Match& match) const noexcept {
            switch (match.id()) {
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
                    const double result = std::fpclassify(divisor) != FP_ZERO ? (evaluate(match.children()[0]) / divisor) : 0;
                    return result;
                }
            }

            assert(false);
            return 0;
        }
    };


} //namespace parserlib::tests


#endif //PARSERLIB_TESTS_CALCULATOR_HPP
