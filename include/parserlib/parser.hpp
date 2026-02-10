#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <stdexcept>
#include "grammar_node.hpp"
#include "parse_context.hpp"


namespace parserlib {


    /**
     * Class that implements the parsing algorithms.
     * Adding new parsing algorithms or replacing the old ones 
     * can be achieved by overloading the `parse` method in a subclass.
     * @param ParseContext the parse context type to use.
     * @param SymbolComparator the symbol comparator to use.
     */ 
    template <class ParseContext = parse_context<>, class SymbolComparator = default_symbol_comparator, std::enable_if_t<std::is_base_of_v<parse_context_base, ParseContext>, bool> = true>
    class parser {
    public:
        /**
         * The constructor.
         * @param pc the parse context to use.
         */
        parser(ParseContext& pc)
            : m_parse_context(pc)
        {
        }

        /**
         * Returns the parse context.
         * @return the parse context.
         */ 
        const ParseContext& get_parse_context() const {
            return m_parse_context;
        }

        /**
         * Parses using the given grammar node.
         * Subclasses can be used to either replace existing parsing algorithms,
         * based on the type of the node, or provide new algorithms for new node types.
         * @param node the grammar node to start parsing from.
         * @return true on success, false on failure.
         * @exception std::runtime_error thrown if the node type is unknown.
         */ 
        virtual bool parse(const grammar_node_ptr& node) {
            switch (node->get_type()) {
                case grammar_node_type::grammar_node_type_ref:
                    return _parse(std::static_pointer_cast<ref_grammar_node>(node));

                case grammar_node_type::grammar_node_type_symbol:
                    return _parse(std::static_pointer_cast<symbol_grammar_node>(node));

                case grammar_node_type::grammar_node_type_string:
                    return _parse(std::static_pointer_cast<string_grammar_node>(node));

                case grammar_node_type::grammar_node_type_set:
                    return _parse(std::static_pointer_cast<set_grammar_node>(node));

                case grammar_node_type::grammar_node_type_range:
                    return _parse(std::static_pointer_cast<range_grammar_node>(node));

                case grammar_node_type::grammar_node_type_loop_0:
                    return _parse(std::static_pointer_cast<loop_0_grammar_node>(node));

                case grammar_node_type::grammar_node_type_loop_1:
                    return _parse(std::static_pointer_cast<loop_1_grammar_node>(node));

                case grammar_node_type::grammar_node_type_optional:
                    return _parse(std::static_pointer_cast<optional_grammar_node>(node));

                case grammar_node_type::grammar_node_type_logical_and:
                    return _parse(std::static_pointer_cast<logical_and_grammar_node>(node));

                case grammar_node_type::grammar_node_type_logical_not:
                    return _parse(std::static_pointer_cast<logical_not_grammar_node>(node));

                case grammar_node_type::grammar_node_type_sequence:
                    return _parse(std::static_pointer_cast<sequence_grammar_node>(node));

                case grammar_node_type::grammar_node_type_choice:
                    return _parse(std::static_pointer_cast<choice_grammar_node>(node));

                case grammar_node_type::grammar_node_type_match:
                    return _parse(std::static_pointer_cast<match_grammar_node>(node));

                case grammar_node_type::grammar_node_type_any:
                    return _parse(std::static_pointer_cast<any_grammar_node>(node));

                case grammar_node_type::grammar_node_type_end:
                    return _parse(std::static_pointer_cast<end_grammar_node>(node));

                case grammar_node_type::grammar_node_type_false:
                    return _parse(std::static_pointer_cast<false_grammar_node>(node));

                case grammar_node_type::grammar_node_type_true:
                    return _parse(std::static_pointer_cast<true_grammar_node>(node));

                case grammar_node_type::grammar_node_type_function:
                    return _parse(std::static_pointer_cast<function_grammar_node>(node));
            }

            throw std::runtime_error(std::string("parser: unhandled grammar node type: ") + std::to_string(static_cast<symbol_value_type>(node->get_type())));
        }

    private:
        //the parse context to work with
        ParseContext& m_parse_context;

        //invokes the symbol
        template <class L, class R>
        static symbol_value_type _compare(const L& left, const R& right) {
            return SymbolComparator::compare(static_cast<symbol_value_type>(left), static_cast<symbol_value_type>(right));
        }

        //parse the node the ref refers to
        bool _parse(const std::shared_ptr<ref_grammar_node>& node) {
            return parse(node->get_node());
        }

        //check if the symbol matches the source
        bool _parse(const std::shared_ptr<symbol_grammar_node>& node) {
            if (m_parse_context.is_valid_iterator()) {
                const auto& source_symbol = *m_parse_context.get_iterator();
                const int grammar_symbol = node->get_symbol()->get_value();
                if (_compare(source_symbol, grammar_symbol) == 0) {
                    m_parse_context.increment_iterator();
                    return true;
                }
            }
            return false;
        }

        //check if the string matches the source
        bool _parse(const std::shared_ptr<string_grammar_node>& node) {
            auto itSource = m_parse_context.get_iterator();
            const auto& symbol_value = node->get_symbol()->get_value();
            auto itGrammar = symbol_value.begin();
            const auto itGrammarEnd = symbol_value.end();
            for (;;) {
                if (itGrammar == itGrammarEnd) {
                    m_parse_context.increment_iterator(symbol_value.size());
                    return true;
                }
                if (itSource == m_parse_context.get_end_iterator()) {
                    break;
                }
                if (_compare(*itSource, *itGrammar) != 0) {
                    break;
                }
                ++itSource;
                ++itGrammar;
            }
            return false;
        }

        //check if the source symbol is in the set
        bool _parse(const std::shared_ptr<set_grammar_node>& node) {
            if (m_parse_context.is_valid_iterator()) {
                const auto& source_symbol = *m_parse_context.get_iterator();
                const auto& symbol_value = node->get_symbol()->get_value();
                auto it = std::upper_bound(symbol_value.begin(), symbol_value.end(), source_symbol, [&](const auto& a, const auto& b) {
                    return _compare(a, b) < 0;
                });
                if (it != symbol_value.begin()) {
                    --it;
                    if (_compare(source_symbol, *it) == 0) {
                        m_parse_context.increment_iterator();
                        return true;
                    }
                }
            }
            return false;
        }

        //checks if the source symbol is within the range
        bool _parse(const std::shared_ptr<range_grammar_node>& node) {
            if (m_parse_context.is_valid_iterator()) {
                const auto& source_symbol = *m_parse_context.get_iterator();
                const auto& symbol_value = node->get_symbol()->get_value();
                if (_compare(source_symbol, symbol_value.first) >= 0 && _compare(source_symbol, symbol_value.second) <= 0) {
                    m_parse_context.increment_iterator();
                    return true;
                }
            }
            return false;
        }

        //parse until there is an error
        bool _parse(const std::shared_ptr<loop_0_grammar_node>& node) {
            while (_parse_and_restore_state_on_failure(node->get_child())) {
            }
            return true;
        }

        //parse once, then parse in loop
        bool _parse(const std::shared_ptr<loop_1_grammar_node>& node) {
            if (parse(node->get_child())) {
                while (_parse_and_restore_state_on_failure(node->get_child())) {
                }
                return true;
            }
            return false;
        }

        //parse, then return true no matter what the result was
        bool _parse(const std::shared_ptr<optional_grammar_node>& node) {
            _parse_and_restore_state_on_failure(node->get_child());
            return true;
        }

        //parse then restore state
        bool _parse(const std::shared_ptr<logical_and_grammar_node>& node) {
            return _parse_and_restore_state(node->get_child());
        }

        //parse then restore state, reverse the result
        bool _parse(const std::shared_ptr<logical_not_grammar_node>& node) {
            return !_parse_and_restore_state(node->get_child());
        }

        //parse all in sequence
        bool _parse(const std::shared_ptr<sequence_grammar_node>& node) {
            const auto prev_state = m_parse_context.get_state();
            for (const grammar_node_ptr& child : node->get_children()) {
                try {
                    if (!parse(child)) {
                        m_parse_context.set_state(prev_state);
                        return false;
                    }
                }
                catch (...) {
                    m_parse_context.set_state(prev_state);
                    throw;
                }
            }
            return true;
        }

        //parse one after the other, return for the first branch that it succeeds
        bool _parse(const std::shared_ptr<choice_grammar_node>& node) {
            for (const grammar_node_ptr& child : node->get_children()) {
                if (_parse_and_restore_state_on_failure(child)) {
                    return true;
                }
            }
            return false;
        }

        //add a match if subnode succeeds
        bool _parse(const std::shared_ptr<match_grammar_node>& node) {
            const auto from_state = m_parse_context.get_state();
            if (parse(node->get_child())) {
                m_parse_context.add_match(static_cast<typename ParseContext::match_id>(node->get_id_symbol()->get_value()), from_state);
                return true;
            }
            return false;
        }

        //advance one symbol
        bool _parse(const std::shared_ptr<any_grammar_node>& node) {
            if (m_parse_context.is_valid_iterator()) {
                m_parse_context.increment_iterator();
                return true;
            }
            return false;
        }

        //check for end
        bool _parse(const std::shared_ptr<end_grammar_node>& node) {
            return m_parse_context.is_end_iterator();
        }

        //return false
        bool _parse(const std::shared_ptr<false_grammar_node>& node) {
            return false;
        }

        //return true
        bool _parse(const std::shared_ptr<true_grammar_node>& node) {
            return true;
        }

        //invoke the function with the parse context
        bool _parse(const std::shared_ptr<function_grammar_node>& node) {
            return node->get_function()(m_parse_context);
        }

        //if parsing fails, restore the parse context state
        bool _parse_and_restore_state_on_failure(const grammar_node_ptr& node) {
            const auto prev_state = m_parse_context.get_state();
            try {
                const bool result = parse(node);
                if (result) {
                    return true;
                }
                m_parse_context.set_state(prev_state);
            }
            catch (...) {
                m_parse_context.set_state(prev_state);
                throw;
            }
            return false;
        }

        //restore the parse context state anyway
        bool _parse_and_restore_state(const grammar_node_ptr& node) {
            const auto prev_state = m_parse_context.get_state();
            try {
                const bool result = parse(node);
                m_parse_context.set_state(prev_state);
                return result;
            }
            catch (...) {
                m_parse_context.set_state(prev_state);
                throw;
            }
            return false;
        }
    };


    template <class ParseContext, class SymbolComparator>
    inline bool grammar_node_ptr::parse(ParseContext& pc) const {
        parser<ParseContext, SymbolComparator> parser(pc);
        return parser.parse(*this);
    }


    template <class ParseContext, class SymbolComparator>
    inline bool rule::parse(ParseContext& pc) const {
        return get_node().parse<ParseContext, SymbolComparator>(pc);
    }


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
