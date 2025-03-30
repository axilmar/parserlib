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


void test_utility() {
    test_load_file();
}
