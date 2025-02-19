#include <sstream>
#include <iostream>
#include <cassert>
#include "parserlib/stream_container.hpp"


using namespace parserlib;


void test_stream_container() {
    {
        std::stringstream stream1, stream2;
        stream1 << "12345";
        stream_container<std::stringstream> container(stream1);
        for (auto it = container.begin(); it != container.end(); ++it) {
            stream2 << *it;
        }
        assert(stream1.str() == stream2.str());
    }
}
