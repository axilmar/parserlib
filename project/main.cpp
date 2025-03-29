extern void test_parse_context();
extern void test_terminal_parse_nodes();
extern void test_unary_parse_nodes();
extern void test_binary_parse_nodes();
extern void test_other_parse_nodes();
extern void test_rule();


static void run_tests() {
    test_parse_context();
    test_terminal_parse_nodes();
    test_unary_parse_nodes();
    test_binary_parse_nodes();
    test_other_parse_nodes();
    test_rule();
}


int main() {
    run_tests();
    return 0;
}
