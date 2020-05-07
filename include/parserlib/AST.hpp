#ifndef PARSERLIB_AST_HPP
#define PARSERLIB_AST_HPP


#include "Rule.hpp"


namespace parserlib
{


    /**
        Helper class for declaring connections of AST to rules.
        @param NodeType type of node to create.
     */
    template <typename NodeType> class AST
    {
    public:
        /**
            Constructor.
            @param rule rule to connect to this AST.
            @param rules optional list of other rules to connect to this AST.
         */
        template <typename ParseContextType, typename ...Rules>
        AST(Rule<ParseContextType>& rule, Rules&... rules)
        {
            registerAST(rule, rules...);
        }

    private:
        //register one rule
        template <typename ParseContextType>
        static void registerAST(Rule<ParseContextType>& rule)
        {
            rule.setCallback([](
                const typename ParseContextType::MatchType &match,
                ASTNodeStack& ans)
            {
                ans.push_back(std::make_shared<NodeType>(match, ans));
            });
        }

        //register many rules
        template <typename ParseContextType, typename ...Rules>
        static void registerAST(Rule<ParseContextType>& rule, Rules&... rules)
        {
            registerAST(rule);
            registerAST(rules...);
        }
    };


    /**
        Helper function for creating an AST.
        Objects are create bottom-up, i.e. the inner most objects are created first,
        then their parents, which pick their children from a stack.
        @param parseContext parse context after successful parsing.
        @return pointer to the root object.
     */
    template <typename ResultType, typename ParseContextType>
    std::shared_ptr<ResultType> createAST(const ParseContextType &parseContext)
    {
        ASTNodeStack asn;
        for (const auto& match : parseContext.getOutput())
        {
            static_cast<const Rule<ParseContextType> &>(match.getRule()).getCallback()(match, asn);
        }
        return std::dynamic_pointer_cast<ResultType>(asn.back());
    }


} //namespace parserlib


#endif //PARSERLIB_AST_HPP
