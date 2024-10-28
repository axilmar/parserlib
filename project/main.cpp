

extern void test_parser_engine();
extern void test_lexer();


static void test() {
    test_parser_engine();
    test_lexer();
}


int main() {
    test();
    return 0;
}
