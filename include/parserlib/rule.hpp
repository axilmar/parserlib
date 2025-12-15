#ifndef PARSERLIB_RULE_HPP
#define PARSERLIB_RULE_HPP


#include <memory>
#include <map>
#include <utility>
#include "parse_node.hpp"
#include "parse_node_wrapper.hpp"
#include "parse_left_recursion_algorithm.hpp"
#include "parse_context.hpp"


namespace parserlib {


    /**
     * Special parse node that allows the creation of recursive grammars.
     * @param ParseContext the parse context to use for parsing.
     */
    template <class ParseContext = parse_context>
    class rule : public parse_node<rule<ParseContext>> {
    public:
        /**
         * The default constructor.
         */
        rule() 
            : m_forward_reference(get_forward_reference(this))
        {
        }

        /**
         * The copy constructor.
         * @param src the source object.
         */
        rule(const rule& src)
            : m_forward_reference(get_forward_reference(std::addressof(src)))
        {
            register_forward_reference();
        }

        /**
         * The move constructor.
         * @param src the source object.
         */
        rule(rule&& src)
            : m_forward_reference(std::move(src.m_forward_reference))
        {
            register_forward_reference();
        }

        /**
         * Constructor from value/parse node.
         * @param src source value/parse node.
         */
        template <class T>
        rule(const T& src)
            : m_forward_reference(get_forward_reference(this))
        {            
            m_forward_reference->parse_node = make_unique_wrapper(src);
        }

        /**
         * The copy assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        rule& operator = (const rule& src) {
            m_forward_reference = get_forward_reference(std::addressof(src));
            register_forward_reference();
            return *this;
        }

        /**
         * The move assignment operator.
         * @param src the source object.
         * @return reference to this.
         */
        rule& operator = (rule&& src) {
            m_forward_reference = std::move(src.m_forward_reference);
            register_forward_reference();
            return *this;
        }

        /**
         * Assignment from value/parse node.
         * All instances of this rule receive the new parse node.
         * @param src the source object.
         * @return reference to this.
         */
        template <class T>
        rule& operator = (const T& src) {
            m_forward_reference->parse_node = make_unique_wrapper(src);
            return *this;
        }

        /**
         * Parses this rule using left recursion parsing.
         * @param pc the parse context to use.
         * @return true on success, false on failure.
         */
        bool parse(ParseContext& pc) const {
            return parse_left_recursion_algorithm::parse(
                pc, 
                reinterpret_cast<parse_node_id_type>(m_forward_reference->parse_node.get()), 
                *m_forward_reference->parse_node.get()
            );
        }

    private:
        //each rule points to this shared object
        struct forward_reference {
            std::unique_ptr<parse_node_wrapper<ParseContext>> parse_node;
        };

        //the shared object
        std::shared_ptr<forward_reference> m_forward_reference;

        //type of map for storing forward references
        using forward_reference_map = std::map<const rule*, std::shared_ptr<forward_reference>>;

        //contains the forward references map
        static forward_reference_map& get_forward_reference_map() {
            static thread_local forward_reference_map forward_references;
            return forward_references;
        }

        //get a forward reference for the given rule;
        //if the forward reference struct does not exist,
        //then a new one is created.
        static std::shared_ptr<forward_reference> get_forward_reference(const rule* r) {
            forward_reference_map& forward_references = get_forward_reference_map();
            auto it = forward_references.find(r);
            if (it != forward_references.end()) {
                return it->second;
            }
            auto [it1, ok] = forward_references.insert(std::make_pair(r, std::make_shared<forward_reference>()));
            return it1->second;
        }

        //creates a unique wrapper for type T
        template <class T>
        static std::unique_ptr<parse_node_wrapper<ParseContext>> make_unique_wrapper(const T& src) {
            using parse_node_type = decltype(make_parse_node(src));
            using wrapper_type = parse_node_wrapper_impl<ParseContext, parse_node_type>;
            return std::make_unique<wrapper_type>(make_parse_node(src));
        }

        //registers this with its current forward reference
        void register_forward_reference() {
            get_forward_reference_map()[this] = m_forward_reference;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
