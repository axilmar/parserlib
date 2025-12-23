#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <utility>
#include <map>
#include "parse_node_wrapper.hpp"
#include "parse_left_recursion_algorithm.hpp"


namespace parserlib {


    /**
     * A parse node that allows grammars to be recursive.
     * @param ParseContext type of parse context to use.
     */
    template <class ParseContext>
    class rule : public parse_node<rule<ParseContext>> {
    public:
        /**
         * The default constructor.
         */
        rule() 
            : m_ref(make_ref(this))
        {
        }

        /**
         * The copy constructor.
         * @param r the source object.
         */
        rule(const rule& r)
            : m_ref(make_ref(std::addressof(r)))
        {
        }

        rule(rule&& r) = delete;

        /**
         * Constructor from value/parse node.
         * @param value value/parse node.
         */
        template <class T>
        rule(const T& value)
            : m_ref(make_ref(this, make_wrapper(value)))
        {
        }

        /**
         * The copy assignment operator.
         * @param r the source object.
         * @return reference to this.
         */
        rule& operator = (const rule& r) {
            m_ref = make_ref(std::addressof(r));
            return *this;
        }

        rule& operator = (rule&& r) = delete;

        /**
         * Assignment from value/parse node.
         * @param value value/parse node.
         * @return reference to this.
         */
        template <class T>
        rule& operator = (const T& value) {
            m_ref->parse_node = make_wrapper(value);
            return *this;
        }

        /**
         * Parses using the left recursion parsing algorithm.
         * @param pc the parse context.
         * @return true on success, false on failure.
         */
        bool parse(ParseContext& pc) const {
            const parse_node_wrapper<ParseContext>& pn = *m_ref->parse_node;
            return parse_left_recursion_algorithm::parse(pc, pn);
        }

    private:
        //reference
        struct ref {
            //pointer to parse node
            std::unique_ptr<parse_node_wrapper<ParseContext>> parse_node;

            //constructor
            ref(std::unique_ptr<parse_node_wrapper<ParseContext>>&& ptr)
                : parse_node(std::move(ptr))
            {
            }
        };

        //pointer to reference
        std::shared_ptr<ref> m_ref;

        //creates a wrapper for the given type.
        template <class T>
        static std::unique_ptr<parse_node_wrapper<ParseContext>> make_wrapper(const T& value) {
            return std::make_unique<parse_node_wrapper_impl<ParseContext, decltype(make_parse_node(value))>>(make_parse_node(value));
        }

        //creates a reference
        static std::shared_ptr<ref> make_ref(const rule* r, std::unique_ptr<parse_node_wrapper<ParseContext>>&& pn = {}) {
            static thread_local std::map<const rule*, std::shared_ptr<ref>> refs;
            const auto it = refs.find(r);
            if (it != refs.end()) {
                it->second->parse_node = std::move(pn);
                return it->second;
            }
            return refs[r] = std::make_shared<ref>(std::move(pn));
        }
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
