

extern void test_parser_engine();
extern void test_lexer();
extern void test_utf8_string();


static void test() {
    test_parser_engine();
    test_lexer();
    test_utf8_string();
}


int main() {
    test();
    return 0;
}
