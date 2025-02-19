extern void test_parser_classes();
extern void test_stream_container();
extern void test_parse_context();


static void test() {
    test_parser_classes();
    test_stream_container();
    test_parse_context();
}


int main() {
    test();
    return 0;
}
