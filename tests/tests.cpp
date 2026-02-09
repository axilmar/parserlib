#include <cassert>
#include "parserlib.hpp"


using namespace parserlib;


class test_node : public single_linked_list_node<test_node> {
public:
    int value;
    test_node(int v) : value(v) {}
};


static void test_singled_linked_list() {
    //test 'next'
    std::shared_ptr<test_node> node1 = std::make_shared<test_node>(1);
    std::shared_ptr<test_node> node2 = std::make_shared<test_node>(2);
    node1->set_next(node2);
    assert(node1->get_next() == node2);

    //test 'last'
    std::shared_ptr<test_node> node3 = std::make_shared<test_node>(3);
    node1->set_last(node3);
    assert(node1->get_last() == node3);

}


static void test_symbol() {
    symbol_ptr symbol1 = std::make_unique<typed_symbol<char>>('a');
    assert(symbol1->get_value() == 'a');

    std::string_view str1("abc");
    symbol_string_ptr symbol2 = std::make_unique<typed_symbol_string<char>>(str1);
    assert(symbol2->get_value() == std::vector<symbol_value_type>(str1.begin(), str1.end()));

    symbol_set_ptr symbol3 = std::make_unique<typed_symbol_set<char>>("9467182035");
    assert(symbol3->get_value() == std::vector<symbol_value_type>({ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' }));

    symbol_pair_ptr symbol4 = std::make_unique<typed_symbol_pair<char, char>>('0', '9');
    assert(symbol4->get_value() == std::make_pair(symbol_value_type('0'), symbol_value_type('9')));
}


static void test_parse_symbol() {
    const rule grammar = 'a';

    //true
    {
        std::string src = "a";
        parse_context pc(src);
        parser p(pc);
        const bool ok = p.parse(grammar);
        assert(ok);
        assert(pc.get_iterator() == src.end());
    }

    //false
    {
        std::string src = "b";
        parse_context pc(src);
        parser p(pc);
        const bool ok = p.parse(grammar);
        assert(!ok);
        assert(pc.get_iterator() == src.begin());
    }
}


void run_tests() {
    test_singled_linked_list();
    test_symbol();
    test_parse_symbol();
}
