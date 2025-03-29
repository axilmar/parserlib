#include <cassert>
#include <cstdio>
#include "parserlib/utility.hpp"


using namespace parserlib;


const std::string TEST_STRING = "0123456789";


static std::string write_test_file() {
    const std::string path = "test.txt";
    std::ofstream file(path);
    file << TEST_STRING;
    return path;
}


static void delete_test_file(const std::string& path) {
    std::remove(path.c_str());
}


static void test_load_file() {
    const std::string path = write_test_file();
    std::string str;
    assert(load_file(path, str));
    delete_test_file(path);
    assert(str == TEST_STRING);
}


static void test_get_source_positions() {
    std::string source =
        "1\n"
        "\n"
        "2\n"
        "3\n";

    const auto positions = get_source_positions(source);
    assert(positions.size() == 7);
    std::size_t line = 1, column = 1, index = 0;
    for (auto it = source.cbegin(); it != source.cend(); ++it, ++index) {
        if (*it == '\n') {
            ++line;
            column = 1;
        }
        assert(positions[index].line == line);
        assert(positions[index].column == column);
        ++column;
    }
}


void test_utility() {
    test_load_file();
    test_get_source_positions();
}