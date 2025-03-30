extern void test_parse_context();
extern void test_terminal_parse_nodes();
extern void test_unary_parse_nodes();
extern void test_binary_parse_nodes();
extern void test_other_parse_nodes();
extern void test_rule();
extern void test_utility();
extern void test_strings();
extern void test_error_handling();


static void run_tests() {
    test_parse_context();
    test_terminal_parse_nodes();
    test_unary_parse_nodes();
    test_binary_parse_nodes();
    test_other_parse_nodes();
    test_rule();
    test_utility();
    test_strings();
    test_error_handling();
}


int main() {
    run_tests();
    return 0;
}
