extern void test_parse_context();
extern void test_terminal_parse_nodes();
extern void test_unary_parse_nodes();


static void run_tests() {
    test_parse_context();
    test_terminal_parse_nodes();
    test_unary_parse_nodes();
}


int main() {
    run_tests();
    return 0;
}
