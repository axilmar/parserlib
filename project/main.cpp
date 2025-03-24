extern void test_parser_classes();
extern void test_stream_wrapper();
extern void test_parse_context();
extern void test_custom_token_parsing();
extern void test_string_wrapper();
extern void test_ast_node();
extern void test_ebnf_grammar();


static void test() {
    test_parser_classes();
    test_stream_wrapper();
    test_parse_context();
    test_custom_token_parsing();
    test_string_wrapper();
    test_ast_node();
    test_ebnf_grammar();
}


int main() {
    test();
    return 0;
}
