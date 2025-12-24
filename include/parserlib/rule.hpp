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
            : m_ref(make_common_ref(std::addressof(r), this))
        {
        }

        /**
         * The move constructor.
         * @param r the source object.
         */
        rule(rule&& r) 
            : m_ref(make_common_ref(std::addressof(r), this))
        {
        }

        /**
         * Constructor from value/parse node.
         * @param value value/parse node.
         */
        template <class T>
        rule(const T& value)
            : m_ref(make_ref_for(this, make_wrapper(value)))
        {
        }

        /**
         * The destructor.
         */
        ~rule() {
            delete_ref(this);
        }

        /**
         * The copy assignment operator.
         * @param r the source object.
         * @return reference to this.
         */
        rule& operator = (const rule& r) {
            m_ref = make_common_ref(std::addressof(r), this);
            return *this;
        }

        /**
         * The move assignment operator.
         * @param r the source object.
         * @return reference to this.
         */
        rule& operator = (rule&& r) {
            m_ref = make_common_ref(std::addressof(r), this);
            return *this;
        }

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

            //the default constructor
            ref() {
            }

            //constructor from parameter
            ref(std::unique_ptr<parse_node_wrapper<ParseContext>>&& ptr)
                : parse_node(std::move(ptr))
            {
            }
        };

        using ref_map = std::map<const rule*, std::shared_ptr<ref>>;

        //pointer to reference
        std::shared_ptr<ref> m_ref;

        //returns the ref map
        static ref_map& get_refs() {
            static thread_local ref_map rm;
            return rm;
        }

        //creates a wrapper for the given type.
        template <class T>
        static std::unique_ptr<parse_node_wrapper<ParseContext>> make_wrapper(const T& value) {
            return std::make_unique<parse_node_wrapper_impl<ParseContext, decltype(make_parse_node(value))>>(make_parse_node(value));
        }

        //creates a reference for one rule
        static std::shared_ptr<ref> make_ref(const rule* r) {
            //get the ref map
            ref_map& refs = get_refs();

            //find the ref
            const auto it = refs.find(r);

            //if found, return it
            if (it != refs.end()) {
                return it->second;
            }

            //create a new empty ref
            std::shared_ptr<ref> result = std::make_shared<ref>();

            //register the new ref
            refs[r] = result;

            //return the new ref
            return result;
        }

        //creates a reference for one rule with a specific value
        static std::shared_ptr<ref> make_ref_for(const rule* r, std::unique_ptr<parse_node_wrapper<ParseContext>>&& pn) {
            //get the ref map
            ref_map& refs = get_refs();

            //find the ref
            const auto it = refs.find(r);

            //if found, set the parse node and return it
            if (it != refs.end()) {
                it->second->parse_node = std::move(pn);
                return it->second;
            }

            //create a new non-empty ref
            std::shared_ptr<ref> result = std::make_shared<ref>(std::move(pn));

            //register the new ref
            refs[r] = result;

            //return the new ref
            return result;
        }

        //creates a common reference for two rules
        static std::shared_ptr<ref> make_common_ref(const rule* r1, const rule* r2) {
            //make a ref for the first rule
            std::shared_ptr<ref> result = make_ref(r1);

            //also register the same ref for the 2nd rule
            get_refs()[r2] = result;

            //return the result
            return result;
        }

        //deletes a reference
        static void delete_ref(const rule* r) {
            get_refs().erase(r);
        }
    };


} //namespace parserlib


#endif //PARSERLIB_RULE_HPP
