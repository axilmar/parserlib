#ifndef PARSERLIB_PARSER_HPP
#define PARSERLIB_PARSER_HPP


#include <cctype>
#include <stdexcept>
#include "grammar_node.hpp"
#include "parse_context.hpp"


namespace parserlib {


    class default_symbol_comparator {
    public:
        static int compare(int left, int right) {
            return left - right;
        }
    };


    class case_insensitive_symbol_comparator {
    public:
        static int compare(int left, int right) {
            return std::tolower(left) - std::tolower(right);
        }
    };


    template <class ParseContext = parse_context<>, class SymbolComparator = default_symbol_comparator, std::enable_if_t<std::is_base_of_v<parse_context_base, ParseContext>, bool> = true>
    class parser {
    public:
        parser(ParseContext& m_parse_context)
            : m_parse_context(m_parse_context)
        {
        }

        const ParseContext& get_parse_context() const {
            return m_parse_context;
        }

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

            throw std::runtime_error(std::string("parser: unhandled grammar node type: ") + std::to_string(static_cast<int>(node->get_type())));
        }

    private:
        ParseContext& m_parse_context;

        template <class L, class R>
        static int _compare(const L& left, const R& right) {
            return SymbolComparator::compare(static_cast<int>(left), static_cast<int>(right));
        }

        bool _parse(const std::shared_ptr<ref_grammar_node>& node) {
            return parse(node->get_node());
        }

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

        bool _parse(const std::shared_ptr<loop_0_grammar_node>& node) {
            while (_parse_and_restore_state_on_failure(node->get_first())) {
            }
            return true;
        }

        bool _parse(const std::shared_ptr<loop_1_grammar_node>& node) {
            if (parse(node->get_first())) {
                while (_parse_and_restore_state_on_failure(node->get_first())) {
                }
                return true;
            }
            return false;
        }

        bool _parse(const std::shared_ptr<optional_grammar_node>& node) {
            _parse_and_restore_state_on_failure(node->get_first());
            return true;
        }

        bool _parse(const std::shared_ptr<logical_and_grammar_node>& node) {
            return _parse_and_restore_state(node->get_first());
        }

        bool _parse(const std::shared_ptr<logical_not_grammar_node>& node) {
            return !_parse_and_restore_state(node->get_first());
        }

        bool _parse(const std::shared_ptr<sequence_grammar_node>& node) {
            const auto prev_state = m_parse_context.get_state();
            for (grammar_node_ptr child = node->get_first(); child; child = child->get_next()) {
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

        bool _parse(const std::shared_ptr<choice_grammar_node>& node) {
            for (grammar_node_ptr child = node->get_first(); child; child = child->get_next()) {
                if (_parse_and_restore_state_on_failure(child)) {
                    return true;
                }
            }
            return false;
        }

        bool _parse(const std::shared_ptr<match_grammar_node>& node) {
            const auto from_state = m_parse_context.get_state();
            if (parse(node->get_first())) {
                m_parse_context.add_match(static_cast<typename ParseContext::match_id>(node->get_id_symbol()->get_value()), from_state);
                return true;
            }
            return false;
        }

        bool _parse(const std::shared_ptr<any_grammar_node>& node) {
            if (m_parse_context.is_valid_iterator()) {
                m_parse_context.increment_iterator();
                return true;
            }
            return false;
        }

        bool _parse(const std::shared_ptr<end_grammar_node>& node) {
            return m_parse_context.is_end_iterator();
        }

        bool _parse(const std::shared_ptr<false_grammar_node>& node) {
            return false;
        }

        bool _parse(const std::shared_ptr<true_grammar_node>& node) {
            return true;
        }

        bool _parse(const std::shared_ptr<function_grammar_node>& node) {
            return node->get_function()(m_parse_context);
        }

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


} //namespace parserlib


#endif //PARSERLIB_PARSER_HPP
