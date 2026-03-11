#ifndef PARSERLIB_CHOICE_PARSE_NODE_HPP
#define PARSERLIB_CHOICE_PARSE_NODE_HPP


#include <vector>
#include "rule.hpp"


namespace parserlib {


    template <class ParseContext>
    class choice_parse_node : public parse_node<ParseContext> {
    public:
        choice_parse_node(const std::vector<parse_node_ptr<ParseContext>>& parse_nodes)
            : m_parse_nodes(parse_nodes)
        {
        }

        const std::vector<parse_node_ptr<ParseContext>>& get_parse_nodes() const {
            return m_parse_nodes;
        }

        bool parse(ParseContext& pc) const override {
            const typename ParseContext::parse_context_state_type base_state = pc.get_state();

            for (const parse_node_ptr<ParseContext>& parse_node : m_parse_nodes) {
                try {
                    const bool result = parse_node->parse(pc);
                    if (result) {
                        return true;
                    }
                }
                catch (...) {
                    pc.set_state(base_state);
                    throw;
                }

                pc.set_state(base_state);
            }

            return false;
        }

    private:
        std::vector<parse_node_ptr<ParseContext>> m_parse_nodes;
    };


    template <class ParseContext>
    parse_node_ptr<ParseContext> operator | (const parse_node_ptr<ParseContext>& left, const parse_node_ptr<ParseContext>& right) {
        std::vector<parse_node_ptr<ParseContext>> parse_nodes;

        const choice_parse_node<ParseContext>* left_choice = dynamic_cast<const choice_parse_node<ParseContext>*>(left.get());
        if (left_choice) {
            parse_nodes.insert(parse_nodes.end(), left_choice->get_parse_nodes().begin(), left_choice->get_parse_nodes().end());
        }
        else {
            parse_nodes.push_back(left);
        }

        const choice_parse_node<ParseContext>* right_choice = dynamic_cast<const choice_parse_node<ParseContext>*>(right.get());
        if (right_choice) {
            parse_nodes.insert(parse_nodes.end(), right_choice->get_parse_nodes().begin(), right_choice->get_parse_nodes().end());
        }
        else {
            parse_nodes.push_back(right);
        }

        return std::make_shared<choice_parse_node<ParseContext>>(parse_nodes);
    }


    template <class ParseContext, class Symbol>
    parse_node_ptr<ParseContext> operator | (const parse_node_ptr<ParseContext>& left, const Symbol& right) {
        return left | parse_node_ptr<ParseContext>(right);
    }


    template <class ParseContext, class Symbol>
    parse_node_ptr<ParseContext> operator | (const Symbol& left, const parse_node_ptr<ParseContext>& right) {
        return parse_node_ptr<ParseContext>(left) | right;
    }


    template <class ParseContext, class Left>
    parse_node_ptr<ParseContext> operator | (const Left& left, rule<ParseContext>& right) {
        return parse_node_ptr<ParseContext>(left) | parse_node_ptr<ParseContext>(right);
    }


    template <class ParseContext, class Right>
    parse_node_ptr<ParseContext> operator | (rule<ParseContext>& left, const Right& right) {
        return parse_node_ptr<ParseContext>(left) | parse_node_ptr<ParseContext>(right);
    }


} //namespace parserlib


#endif //PARSERLIB_CHOICE_PARSE_NODE_HPP
