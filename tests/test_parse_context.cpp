#include <string>
#include "parserlib/parse_context.hpp"


using namespace parserlib;


void test_parse_context() {
    std::string source = "a";

    enum match_id { A };
    enum error_id { E };

    auto test_comparator = [](const auto& a, const auto& b) noexcept {
        return a - b;
    };

    parse_context<decltype(source), match_id, error_id, decltype(test_comparator)> pc(source);

    assert(&pc.source() == &source);

    const auto start_parse_position = pc.parse_position();
    assert(pc.parse_position() == source.begin());
    assert(pc.is_valid_parse_position());
    
    pc.increment_parse_position();
    assert(pc.is_end_parse_position());

    pc.set_parse_position(start_parse_position);
    assert(pc.parse_position() == start_parse_position);

    pc.increment_parse_position(1);
    assert(pc.is_end_parse_position());
    pc.set_parse_position(start_parse_position);

    auto state0 = pc.state();
    assert(state0.position() == start_parse_position);
    assert(state0.match_count() == 0);

    pc.add_match(match_id::A, source.begin(), source.end(), 0);
    auto state1 = pc.state();
    assert(state1.position() == start_parse_position);
    assert(state1.match_count() == 1);
    assert(pc.matches()[0].id() == match_id::A);
    assert(pc.matches()[0].begin() == source.begin());
    assert(pc.matches()[0].end() == source.end());

    pc.set_state(state0);
    assert(pc.state().position() == start_parse_position);
    assert(pc.state().match_count() == 0);

    assert(pc.errors().size() == 0);
    pc.add_error(error_id::E, source.begin(), source.end());
    assert(pc.errors().size() == 1);
    assert(pc.errors()[0].id() == error_id::E);
    assert(pc.errors()[0].begin() == source.begin());
    assert(pc.errors()[0].end() == source.end());
}
