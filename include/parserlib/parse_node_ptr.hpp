#ifndef PARSERLIB_PARSE_NODE_PTR_HPP
#define PARSERLIB_PARSE_NODE_PTR_HPP


#include <cassert>
#include <memory>
#include <utility>
#include "parse_node.hpp"
#include "parse_context.hpp"
#include "parse_node_wrapper.hpp"


namespace parserlib {


    template <class ParseContext = parse_context>
    class parse_node_ptr : public parse_node<parse_node_ptr<ParseContext>> {
    public:
        parse_node_ptr() {
        }

        parse_node_ptr(const parse_node_ptr& src)
            : m_parse_node(src.m_parse_node)
        {
        }

        parse_node_ptr(parse_node_ptr&& src)
            : m_parse_node(std::move(src.m_parse_node))
        {
        }

        template <class T>
        parse_node_ptr(const T& value)
            : m_parse_node(make_wrapper(make_parse_node(value)))
        {
        }

        parse_node_ptr& operator = (const parse_node_ptr& src) {
            m_parse_node = src.m_parse_node;
            return *this;
        }

        parse_node_ptr& operator = (parse_node_ptr&& src) {
            m_parse_node = std::move(src.m_parse_node);
            return *this;
        }

        template <class T>
        parse_node_ptr& operator = (const T& value) {
            m_parse_node = make_wrapper(make_parse_node(value));
            return *this;
        }

        operator bool() const {
            return m_parse_node;
        }

        parse_node_wrapper<ParseContext>* get() const {
            return m_parse_node.get();
        }

        parse_node_wrapper<ParseContext>* operator ->() const {
            assert(get());
            return get();
        }

        bool parse(ParseContext& pc) const {
            return m_parse_node->parse(pc);
        }

    private:
        std::shared_ptr<parse_node_wrapper<ParseContext>> m_parse_node;

        template <class T>
        std::shared_ptr<parse_node_wrapper<ParseContext>> make_wrapper(const parse_node<T>& parse_node) {
            return std::make_shared<parse_node_wrapper_impl<ParseContext, T>>(*parse_node.get_impl());
        }
    };


    template <class Impl>
    auto parse_node<Impl>::operator ~() const {
        return parse_node_ptr<>(*get_impl());
    }


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_PTR_HPP
