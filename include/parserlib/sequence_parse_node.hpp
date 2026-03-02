#ifndef PARSERLIB_SEQUENCE_PARSE_NODE_HPP
#define PARSERLIB_SEQUENCE_PARSE_NODE_HPP


#include <vector>
#include "rule.hpp"


namespace parserlib {


    template <class ParseContext>
    class sequence_parse_node : public parse_node<ParseContext> {
    public:
        sequence_parse_node(const std::vector<parse_node_ptr<ParseContext>>& parse_nodes)
            : m_parse_nodes(parse_nodes)
        {
        }

        const std::vector<parse_node_ptr<ParseContext>>& get_parse_nodes() const {
            return m_parse_nodes;
        }

        bool parse(ParseContext& pc) const override {
            const auto base_state = pc.get_state();
            try {
                for (const parse_node_ptr<ParseContext>& parse_node : m_parse_nodes) {
                    if (!parse_node->parse(pc)) {
                        pc.set_state(base_state);
                        return false;
                    }
                }
            }
            catch (...) {
                pc.set_state(base_state);
                throw;
            }
            return true;
        }

    private:
        std::vector<parse_node_ptr<ParseContext>> m_parse_nodes;
    };


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator >> (const parse_node_ptr<ParseContext>& left, const parse_node_ptr<ParseContext>& right) {
        std::vector<parse_node_ptr<ParseContext>> parse_nodes;

        const sequence_parse_node<ParseContext>* left_sequence = dynamic_cast<const sequence_parse_node<ParseContext>*>(left.get());
        if (left_sequence) {
            parse_nodes.insert(parse_nodes.end(), left_sequence->get_parse_nodes().begin(), left_sequence->get_parse_nodes().end());
        }
        else {
            parse_nodes.push_back(left);
        }

        const sequence_parse_node<ParseContext>* right_sequence = dynamic_cast<const sequence_parse_node<ParseContext>*>(right.get());
        if (right_sequence) {
            parse_nodes.insert(parse_nodes.end(), right_sequence->get_parse_nodes().begin(), right_sequence->get_parse_nodes().end());
        }
        else {
            parse_nodes.push_back(right);
        }

        return std::make_shared<sequence_parse_node<ParseContext>>(parse_nodes);
    }


    template <class ParseContext, class Symbol>
    parse_node_ptr<ParseContext> operator >> (const parse_node_ptr<ParseContext>& left, const Symbol& right) {
        return left >> parse_node_ptr<ParseContext>(right);
    }


    template <class ParseContext, class Symbol>
    parse_node_ptr<ParseContext> operator >> (const Symbol& left, const parse_node_ptr<ParseContext>& right) {
        return parse_node_ptr<ParseContext>(left) >> right;
    }


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator - (const parse_node_ptr<ParseContext>& left, const parse_node_ptr<ParseContext>& right) {
        return !right >> left;
    }


    template <class ParseContext, class Left>
    parse_node_ptr<ParseContext> operator >> (const Left& left, rule<ParseContext>& right) {
        return parse_node_ptr<ParseContext>(left) >> parse_node_ptr<ParseContext>(right);
    }


    template <class ParseContext, class Right>
    parse_node_ptr<ParseContext> operator >> (rule<ParseContext>& left, const Right& right) {
        return parse_node_ptr<ParseContext>(left) >> parse_node_ptr<ParseContext>(right);
    }


    template <class ParseContext, class Symbol>
    parse_node_ptr<ParseContext> operator - (const parse_node_ptr<ParseContext>& left, const Symbol& right) {
        return left - parse_node_ptr<ParseContext>(right);
    }


    template <class ParseContext, class Symbol>
    parse_node_ptr<ParseContext> operator - (const Symbol& left, const parse_node_ptr<ParseContext>& right) {
        return parse_node_ptr<ParseContext>(left) - right;
    }


    template <class Left, class ParseContext>
    parse_node_ptr<ParseContext> operator - (const Left& left, rule<ParseContext>& right) {
        return parse_node_ptr<ParseContext>(left) - parse_node_ptr<ParseContext>(right);
    }


    template <class ParseContext, class Right>
    parse_node_ptr<ParseContext> operator - (rule<ParseContext>& left, const Right& right) {
        return parse_node_ptr<ParseContext>(left) - parse_node_ptr<ParseContext>(right);
    }


} //namespace parserlib


#endif //PARSERLIB_SEQUENCE_PARSE_NODE_HPP
