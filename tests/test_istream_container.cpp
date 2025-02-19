#include <sstream>
#include <iostream>
#include <cassert>
#include "parserlib/istream_container.hpp"


using namespace parserlib;


void test_istream_container() {
    {
        std::stringstream stream1, stream2;
        stream1 << "12345";
        istream_container<std::stringstream::char_type, std::stringstream::traits_type> container(stream1);
        for (auto it = container.begin(); it != container.end(); ++it) {
            stream2 << *it;
        }
        assert(stream1.str() == stream2.str());
    }
}
