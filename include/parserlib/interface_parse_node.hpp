#ifndef PARSERLIB_INTERFACE_PARSE_NODE_HPP
#define PARSERLIB_INTERFACE_PARSE_NODE_HPP


#include <memory>
#include <string>
#include "interface_parse_context.hpp"


namespace parserlib::interface {


    class parse_node {
    public:
        virtual ~parse_node() {
        }

        const std::string& get_name() const {
            return m_name;
        }

        void set_name(const std::string& name) {
            m_name = name;
        }

        virtual bool parse(interface::parse_context& pc) const = 0;

    private:
        std::string m_name;
    };


    using parse_node_ptr = std::shared_ptr<parse_node>;


    template <class T, class... Args>
    parse_node_ptr create_parse_node(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }


} //namespace parserlib::interface


#endif //PARSERLIB_INTERFACE_PARSE_NODE_HPP
