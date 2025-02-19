extern void test_parser_classes();
extern void test_istream_container();
extern void test_parse_context();


static void test() {
    test_parser_classes();
    test_istream_container();
    test_parse_context();
}


int main() {
    test();
    return 0;
}
