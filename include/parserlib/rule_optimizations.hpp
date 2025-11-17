#ifndef PARSERLIB_RULE_OPTIMIZATIONS_HPP
#define PARSERLIB_RULE_OPTIMIZATIONS_HPP


#include "sequence_parse_node.hpp"
#include "choice_parse_node.hpp"
#include "match_parse_node.hpp"
#include "rule_ref_parse_node.hpp"
#include "annotation_parse_node.hpp"
#include "parse_node_wrapper.hpp"
#include "tuple.hpp"


namespace parserlib {


    template <class ParseNode>
    class left_recursive_match_parse_node: public parse_node<left_recursive_match_parse_node<ParseNode>>{
    public:
        left_recursive_match_parse_node(const ParseNode& parse_node) 
            : m_parse_node(parse_node)
            #ifndef NDEBUG
            , m_text("(" + m_parse_node.text())
            #endif
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            const auto prev_lr_start_state = pc.left_recursion_start_state();
            pc.set_left_recursion_start_state(pc.get_state());
            try {
                const bool result = m_parse_node.parse(pc);
                pc.set_left_recursion_start_state(prev_lr_start_state);
                return result;
            }
            catch (...) {
                pc.set_left_recursion_start_state(prev_lr_start_state);
                throw;
            }
        }

        #ifndef NDEBUG
        const std::string& text() const {
            return m_text;
        }
        #endif

    private:
        const ParseNode m_parse_node;
        #ifndef NDEBUG
        const std::string m_text;
        #endif
    };


    template <class MatchId> 
    class match_end_parse_node : public parse_node<match_end_parse_node<MatchId>> {
    public:
        match_end_parse_node(const MatchId& id) 
            : m_id(id)
            #ifndef NDEBUG
            , m_text(create_text())
            #endif
        {
        }

        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            pc.add_match(m_id, pc.left_recursion_start_state(), pc.iterator());
            return true;
        }

        #ifndef NDEBUG
        const std::string& text() const {
            return m_text;
        }
        #endif

    private:
        const MatchId m_id;
        #ifndef NDEBUG
        const std::string m_text;
        #endif

        #ifndef NDEBUG
        std::string create_text() const {
            std::stringstream stream;
            stream << ") ->* " << id_name<MatchId>::get(m_id);
            return stream.str();
        }
        #endif
    };


    template <class ParseContext, class ParseNode>
    auto optimize_rule_parse_node(rule<ParseContext>& r, ParseNode&& parse_node) {
        return make_unique_parse_node_wrapper<ParseContext>(parse_node);
    }


    template <class ParseContext, class... Sequence1Tail, class... ChoiceTail>
    auto optimize_rule_parse_node(
        rule<ParseContext>& r, 
        const choice_parse_node<
            sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence1Tail...>, 
            ChoiceTail...
        >& choice)
    {
        const auto& sequence1 = std::get<0>(choice.children());
        const auto& rule_ref1 = std::get<0>(sequence1.children());

        if (rule_ref1.rule().is_same(r)) {
            const auto expr =
                choice_parse_node(get_tuple_right<1>(choice.children())) >>
                *sequence_parse_node(get_tuple_right<1>(sequence1.children()));

            return make_unique_parse_node_wrapper<ParseContext>(expr);
        }

        return make_unique_parse_node_wrapper<ParseContext>(choice);
    }


    template <class ParseContext, class... Sequence1Tail, class... Sequence2Tail, class... ChoiceTail>
    auto optimize_rule_parse_node(
        rule<ParseContext>& r,
        const choice_parse_node<
        sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence1Tail...>,
        sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence2Tail...>,
        ChoiceTail...
        >& choice)
    {
        const auto& sequence1 = std::get<0>(choice.children());
        const auto& rule_ref1 = std::get<0>(sequence1.children());

        const auto& sequence2 = std::get<1>(choice.children());
        const auto& rule_ref2 = std::get<0>(sequence2.children());

        if (rule_ref1.rule().is_same(r) && rule_ref2.rule().is_same(r)) {
            const auto expr =
                choice_parse_node(get_tuple_right<2>(choice.children())) >>
                *(sequence_parse_node(get_tuple_right<1>(sequence1.children())) | 
                  sequence_parse_node(get_tuple_right<1>(sequence2.children())));

            return make_unique_parse_node_wrapper<ParseContext>(expr);
        }

        return make_unique_parse_node_wrapper<ParseContext>(choice);
    }


    template <class ParseContext, class... Sequence1Tail, class... Sequence2Tail, class... Sequence3Tail, class... ChoiceTail>
    auto optimize_rule_parse_node(
        rule<ParseContext>& r,
        const choice_parse_node<
        sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence1Tail...>,
        sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence2Tail...>,
        sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence3Tail...>,
        ChoiceTail...
        >& choice)
    {
        const auto& sequence1 = std::get<0>(choice.children());
        const auto& rule_ref1 = std::get<0>(sequence1.children());

        const auto& sequence2 = std::get<1>(choice.children());
        const auto& rule_ref2 = std::get<0>(sequence2.children());

        const auto& sequence3 = std::get<2>(choice.children());
        const auto& rule_ref3 = std::get<0>(sequence3.children());

        if (rule_ref1.rule().is_same(r) && rule_ref2.rule().is_same(r) && rule_ref3.rule().is_same(r)) {
            const auto expr =
                choice_parse_node(get_tuple_right<2>(choice.children())) >>
                *(sequence_parse_node(get_tuple_right<1>(sequence1.children())) |
                  sequence_parse_node(get_tuple_right<1>(sequence2.children())) |
                  sequence_parse_node(get_tuple_right<1>(sequence3.children())));

            return make_unique_parse_node_wrapper<ParseContext>(expr);
        }

        return make_unique_parse_node_wrapper<ParseContext>(choice);
    }


    template <class ParseContext, class MatchId, class... Sequence1Tail, class... ChoiceTail>
    auto optimize_rule_parse_node(
        rule<ParseContext>& r,
        const choice_parse_node<
        match_parse_node<MatchId, sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence1Tail...>>,
        ChoiceTail...
        >& choice)
    {
        const auto& match1 = std::get<0>(choice.children());
        const auto& sequence1 = match1.child();
        const auto& rule_ref1 = std::get<0>(sequence1.children());

        if (rule_ref1.rule().is_same(r)) {
            const auto expr =
                left_recursive_match_parse_node(
                    choice_parse_node(get_tuple_right<1>(choice.children())) >>
                    *(sequence_parse_node(get_tuple_right<1>(sequence1.children())) >> match_end_parse_node<MatchId>(match1.id())));

            return make_unique_parse_node_wrapper<ParseContext>(expr);
        }

        return make_unique_parse_node_wrapper<ParseContext>(choice);
    }


    template <class ParseContext, class MatchId, class... Sequence1Tail, class... Sequence2Tail, class... ChoiceTail>
    auto optimize_rule_parse_node(
        rule<ParseContext>& r,
        const choice_parse_node<
        match_parse_node<MatchId, sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence1Tail...>>,
        match_parse_node<MatchId, sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence2Tail...>>,
        ChoiceTail...
        >& choice)
    {
        const auto& match1 = std::get<0>(choice.children());
        const auto& sequence1 = match1.child();
        const auto& rule_ref1 = std::get<0>(sequence1.children());

        const auto& match2 = std::get<1>(choice.children());
        const auto& sequence2 = match2.child();
        const auto& rule_ref2 = std::get<0>(sequence2.children());

        if (rule_ref1.rule().is_same(r) && rule_ref2.rule().is_same(r)) {
            const auto expr =
                left_recursive_match_parse_node(
                    choice_parse_node(get_tuple_right<1>(choice.children())) >>
                    *((sequence_parse_node(get_tuple_right<1>(sequence1.children())) >> match_end_parse_node<MatchId>(match1.id())) |
                      (sequence_parse_node(get_tuple_right<1>(sequence2.children())) >> match_end_parse_node<MatchId>(match2.id()))
                     ));

            return make_unique_parse_node_wrapper<ParseContext>(expr);
        }

        return make_unique_parse_node_wrapper<ParseContext>(choice);
    }


    template <class ParseContext, class MatchId, class... Sequence1Tail, class... Sequence2Tail, class... Sequence3Tail, class... ChoiceTail>
    auto optimize_rule_parse_node(
        rule<ParseContext>& r,
        const choice_parse_node<
        match_parse_node<MatchId, sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence1Tail...>>,
        match_parse_node<MatchId, sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence2Tail...>>,
        match_parse_node<MatchId, sequence_parse_node<rule_ref_parse_node<ParseContext>, Sequence3Tail...>>,
        ChoiceTail...
        >& choice)
    {
        const auto& match1 = std::get<0>(choice.children());
        const auto& sequence1 = match1.child();
        const auto& rule_ref1 = std::get<0>(sequence1.children());

        const auto& match2 = std::get<1>(choice.children());
        const auto& sequence2 = match2.child();
        const auto& rule_ref2 = std::get<0>(sequence2.children());

        const auto& match3 = std::get<2>(choice.children());
        const auto& sequence3 = match3.child();
        const auto& rule_ref3 = std::get<0>(sequence3.children());

        if (rule_ref1.rule().is_same(r) && rule_ref2.rule().is_same(r) && rule_ref3.rule().is_same(r)) {
            const auto expr =
                left_recursive_match_parse_node(
                    choice_parse_node(get_tuple_right<1>(choice.children())) >>
                    *((sequence_parse_node(get_tuple_right<1>(sequence1.children())) >> match_end_parse_node<MatchId>(match1.id())) |
                      (sequence_parse_node(get_tuple_right<1>(sequence2.children())) >> match_end_parse_node<MatchId>(match2.id())) |
                      (sequence_parse_node(get_tuple_right<1>(sequence3.children())) >> match_end_parse_node<MatchId>(match3.id()))
                     ));

            return make_unique_parse_node_wrapper<ParseContext>(expr);
        }

        return make_unique_parse_node_wrapper<ParseContext>(choice);
    }


} //namespace parserlib


#endif //PARSERLIB_RULE_OPTIMIZATIONS_HPP
