#ifndef PARSERLIB_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ERROR_PARSE_NODE_HPP


#include "parse_node.hpp"


namespace parserlib {


    template <class ErrorId>
    class error_parse_node : public interface::parse_node {
    public:
        error_parse_node(const ErrorId& id, const parserlib::parse_node& skip_parse_node) : m_id(id), m_child(skip_parse_node) {
        }

        bool parse(interface::parse_context& pc) const override {
            pc.push_error_start_parse_position();
            try {
                if (m_child.parse(pc)) {
                    pc.add_error(static_cast<int>(m_id));
                    pc.pop_error_start_parse_position();
                    return true;
                }
                pc.pop_error_start_parse_position();
                return false;
            }
            catch (...) {
                pc.pop_error_start_parse_position();
                throw;
            }
        }

    private:
        const ErrorId m_id;
        const parserlib::parse_node m_child;
    };


    template <class ErrorId>
    parse_node error(const ErrorId& id, const parse_node& skip_parse_node = true) {
        return interface::create_parse_node<error_parse_node<ErrorId>>(id, skip_parse_node);
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSE_NODE_HPP
