#include <iostream>
#include "parserlib.hpp"


using namespace parserlib;


class TestParseFunctor : public parse_node<TestParseFunctor> {
public:
    bool parse(parse_context<>& pc) const {
        return false;
    }
};


static bool testParse(parse_context<>& pc) {
    return false;
}


const int arr[4] = {0};


class F1 {
};


class F2 {
public:
    template <class PC>
    bool operator ()(PC& pc) const {
        return pc.parse_valid();
    }
};


const auto l1 = [](auto& pc) { return false; };
const auto l2 = [](parse_context<>& pc) { return false; };
const auto l3 = []() { return false; };


void run_tests() {
    std::cout << typeid(decltype(make_parse_node('b'))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node("aaa"))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(true))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(TestParseFunctor()))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(&testParse))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(testParse))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(arr))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(F1()))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(F2()))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(l1))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(l2))).name() << std::endl;
    std::cout << typeid(decltype(make_parse_node(l3))).name() << std::endl;
}
