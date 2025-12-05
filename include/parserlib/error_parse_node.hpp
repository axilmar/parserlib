#ifndef PARSERLIB_ERROR_PARSE_NODE_HPP
#define PARSERLIB_ERROR_PARSE_NODE_HPP


#include "unary_parse_node.hpp"


namespace parserlib {


    template <class T, class ErrorId>
    class error_parse_node : public unary_parse_node<error_parse_node<T, ErrorId>, T> {
    public:
        error_parse_node(const T& skip_parse_node, const ErrorId& id)
            : unary_parse_node<error_parse_node<T, ErrorId>, T>(skip_parse_node)
            , m_id(id)
        {
        }

        bool parse(parse_context_interface& pc) const {
            pc.push_error_start_state();
            this->child().parse(pc);
            pc.add_error(static_cast<int>(m_id));
            pc.pop_error_start_state();
            return true;
        }

        const ErrorId& id() const {
            return m_id;
        }

    private:
        const ErrorId m_id;
    };


    template <class T, class ErrorId>
    auto error(const ErrorId& id, T&& skip_parse_node) {
        return error_parse_node(make_parse_node(skip_parse_node), id);
    }


} //namespace parserlib


#endif //PARSERLIB_ERROR_PARSE_NODE_HPP
