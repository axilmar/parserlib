extern void test_parser_engine();
extern void test_lexer();
extern void test_utf8_string();
//extern void test_custom_token();


static void test() {
    test_parser_engine();
    test_lexer();
    test_utf8_string();
    //test_custom_token();
}


int main() {
    test();
    return 0;
}
