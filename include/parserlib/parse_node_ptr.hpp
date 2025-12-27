#ifndef PARSERLIB_PARSE_NODE_PTR_HPP
#define PARSERLIB_PARSE_NODE_PTR_HPP


#include <cassert>
#include <memory>
#include <utility>
#include "parse_node.hpp"
#include "parse_node_wrapper.hpp"


namespace parserlib {


    /**
     * Pointer to parse node (using std::shared_ptr for implementation).
     * @param ParseContext parse context type to use.
     */
    template <class ParseContext>
    class parse_node_ptr : public parse_node<parse_node_ptr<ParseContext>> {
    public:
        /**
         * The default constructor.
         */
        parse_node_ptr() {
        }

        /**
         * The copy constructor.
         * @param src the source object.
         */
        parse_node_ptr(const parse_node_ptr& src)
            : m_parse_node(src.m_parse_node)
        {
        }

        /**
         * The move constructor.
         * @param src the source object.
         */
        parse_node_ptr(parse_node_ptr&& src)
            : m_parse_node(std::move(src.m_parse_node))
        {
        }

        /**
         * Constructor from value/parse node.
         * A wrapper class is instantiated on the heap for the given value/parse node.
         * @param value the value or parse node object.
         */
        template <class T>
        parse_node_ptr(const T& value)
            : m_parse_node(make_wrapper(make_parse_node(value)))
        {
        }

        /**
         * The copy assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        parse_node_ptr& operator = (const parse_node_ptr& src) {
            m_parse_node = src.m_parse_node;
            return *this;
        }

        /**
         * The move assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        parse_node_ptr& operator = (parse_node_ptr&& src) {
            m_parse_node = std::move(src.m_parse_node);
            return *this;
        }

        /**
         * Assignment from value/parse node.
         * A wrapper class is instantiated on the heap for the given value/parse node.
         * @param value the value or parse node object.
         */
        template <class T>
        parse_node_ptr& operator = (const T& value) {
            m_parse_node = make_wrapper(make_parse_node(value));
            return *this;
        }

        /**
         * Allows checking if the pointer is empty.
         * @return true if not empty, false otherwise.
         */
        operator bool() const {
            return m_parse_node;
        }

        /**
         * Returns the parse node wrapper.
         * @return the parse node wrapper.
         */
        parse_node_wrapper<ParseContext>* get() const {
            return m_parse_node.get();
        }

        /**
         * Parse node wrapper member access.
         * @return the parse node wrapper.
         */ 
        parse_node_wrapper<ParseContext>* operator ->() const {
            assert(get());
            return get();
        }

        /**
         * Invokes the underlying parse node.
         * @param pc the parse context.
         * @return true on success, false on failure.
         */
        bool parse(ParseContext& pc) const {
            return m_parse_node->parse(pc);
        }

    private:
        std::shared_ptr<parse_node_wrapper<ParseContext>> m_parse_node;

        template <class T>
        static std::shared_ptr<parse_node_wrapper<ParseContext>> make_wrapper(const parse_node<T>& parse_node) {
            return std::make_shared<parse_node_wrapper_impl<ParseContext, T>>(*parse_node.get_impl());
        }
    };


} //namespace parserlib


#endif //PARSERLIB_PARSE_NODE_PTR_HPP
