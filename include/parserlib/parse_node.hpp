#ifndef PARSERLIB_PARSE_NODE_HPP
#define PARSERLIB_PARSE_NODE_HPP


#include <string>
#include <utility>
#include "parse_context_interface.hpp"


namespace parserlib {


    template <class T> class loop_0_parse_node;
    template <class T> class loop_1_parse_node;
    template <class T> class optional_parse_node;
    template <class T> class logical_and_parse_node;
    template <class T> class logical_not_parse_node;


    class parse_node_base {
    public:
        const std::string& name() const {
            return m_name;
        }

        void set_name(const std::string& name) {
            m_name = name;
        }

        const std::string& type() const {
            return m_type;
        }


    protected:
        parse_node_base(const std::string& type)
            : m_type(type)
        {
        }

        parse_node_base(const std::string& name, const std::string& type)
            : m_name(name)
            , m_type(type)
        {
        }

        ~parse_node_base() {
        }

        void set_type(const std::string& type) {
            m_type = type;
        }

    private:
        std::string m_name;
        std::string m_type;
    };


    template <class Derived>
    class parse_node : public parse_node_base {
    public:
        loop_0_parse_node<Derived> operator *() const;

        loop_1_parse_node<Derived> operator +() const;

        optional_parse_node<Derived> operator -() const;

        logical_and_parse_node<Derived> operator &() const;

        logical_not_parse_node<Derived> operator !() const;

    protected:
        parse_node(const std::string& type)
            : parse_node_base(type)
        {
        }

        ~parse_node() {
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_HPP
