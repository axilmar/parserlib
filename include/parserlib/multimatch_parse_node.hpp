#ifndef PARSERLIB_MULTIMATCH_PARSE_NODE_HPP
#define PARSERLIB_MULTIMATCH_PARSE_NODE_HPP


#include "tuple.hpp"
#include "choice_parse_node.hpp"
#include "match_parse_node.hpp"


namespace parserlib {


    /**
     * A multimatch parse node.
     * @param LeadingParseNode the leading parse node type.
     * @param ChoiceParseNode type of the parse node that has choices that lead to matches.
     */
    template <class LeadingParseNode, class ChoiceParseNode>
    class multimatch_parse_node : public parse_node<multimatch_parse_node<LeadingParseNode, ChoiceParseNode>> {
    public:
        /**
         * The constructor.
         * @param leading_parse_node the leading parse node.
         * @param choice_parse_node the choice parse node.
         */
        multimatch_parse_node(const LeadingParseNode& leading_parse_node, const ChoiceParseNode& choice_parse_node)
            : m_leading_parse_node(leading_parse_node)
            , m_choice_parse_node(choice_parse_node)
        {
        }

        /**
         * Establishes the current state as the current multimatch start state,
         * then invokes the leading, then the choice parse node.
         * At return, it restores the previous multimatch start state.
         * @param pc the current parse context.
         * @return true on success, false on failure.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            //set the new multimatch start state
            const auto prev_multimatch_start_state = pc.get_multimatch_start_state();
            pc.set_multimatch_start_state(pc.get_state());

            //parse the leading part
            try {
                if (!m_leading_parse_node.parse(pc)) {
                    pc.set_multimatch_start_state(prev_multimatch_start_state);
                    return false;
                }
            }
            catch (...) {
                pc.set_multimatch_start_state(prev_multimatch_start_state);
                throw;
            }

            //parse the choice part
            try {
                if (!m_choice_parse_node.parse(pc)) {
                    pc.set_multimatch_start_state(prev_multimatch_start_state);
                    return false;
                }
            }
            catch (...) {
                pc.set_multimatch_start_state(prev_multimatch_start_state);
                throw;
            }

            //success
            pc.set_multimatch_start_state(prev_multimatch_start_state);
            return true;
        }

        /**
         * Converts the parse node to a textual description.
         * @return a string of this parse node as text.
         */
        std::string text() const override {
            return "multimatch(" + m_leading_parse_node.text() + ", " + m_choice_parse_node.text() + ')';
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_leading_parse_node.init();
            m_choice_parse_node.init();
        }
        #endif

    private:
        LeadingParseNode m_leading_parse_node;
        ChoiceParseNode m_choice_parse_node;
    };


    template <class MatchId, class ParseNode>
    class multimatch_match_parse_node : public parse_node<multimatch_match_parse_node<MatchId, ParseNode>> {
    public:
        multimatch_match_parse_node(const MatchId& id, const ParseNode& child)
            : m_id(id)
            , m_child(child)
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto match_start_state = pc.get_multimatch_start_state();
            if (pc.parse(m_child)) {
                pc.add_match(m_id, match_start_state, pc.iterator());
                return true;
            }
            return false;
        }

        std::string text() const override {
            std::stringstream stream;
            stream << '(' << m_child.text() << " ->* " << id_name<MatchId>::get(m_id) << ')';
            return stream.str();
        }

        #ifndef NDEBUG
        void init_tree() const override {
            m_child.init();
        }
        #endif

    private:
        const MatchId m_id;
        const ParseNode m_child;
    };


    template <class MatchId, class ParseNode>
    auto make_multimatch_match_parse_node(const match_parse_node<MatchId, ParseNode>& m) {
        return multimatch_match_parse_node(m.id(), m.child());
    }


    template <size_t Index, class...MatchParseNodes>
    auto make_multimatch_tuple(const std::tuple<MatchParseNodes...>& tpl) {
        if constexpr (Index < std::tuple_size_v<std::tuple<MatchParseNodes...>>) {
            return std::tuple_cat(std::make_tuple(make_multimatch_match_parse_node(std::get<Index>(tpl))), make_multimatch_tuple<Index + 1>(tpl));
        }
        else {
            return std::make_tuple();
        }
    }


    template <class... MatchParseNodes>
    auto make_multimatch_choice_parse_node(const std::tuple<MatchParseNodes...>& tpl) {
        return choice_parse_node(make_multimatch_tuple<0>(tpl));
    }


    /**
     * Creates a multimatch structure.
     * @param leading_parse_node the parse node to lead the parsing.
     * @param choices the choice parse node; must contain instances of `match_parse_node`.
     * return a multimatch parse node.
     */
    template <class LeadingParseNode, class... MatchParseNodes>
    auto multimatch(LeadingParseNode&& leading_parse_node, const choice_parse_node<MatchParseNodes...>& choices) {
        return multimatch_parse_node(make_parse_node(leading_parse_node), make_multimatch_choice_parse_node(choices.children()));
    }


} //namespace parserlib


#endif //PARSERLIB_MULTIMATCH_PARSE_NODE_HPP
