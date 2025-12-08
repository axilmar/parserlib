#ifndef PARSERLIB_INTERFACE_PARSE_NODE_HPP
#define PARSERLIB_INTERFACE_PARSE_NODE_HPP


#include <memory>
#include "interface_parse_context.hpp"


namespace parserlib::interface {


    class parse_node {
    public:
        virtual ~parse_node() {
        }

        virtual bool parse(interface::parse_context& pc) const = 0;
    };


    using parse_node_ptr = std::unique_ptr<parse_node>;


    template <class T, class... Args>
    parse_node_ptr create_parse_node(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }


} //namespace parserlib::interface


#endif //PARSERLIB_INTERFACE_PARSE_NODE_HPP
