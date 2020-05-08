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


} //namespace parserlib


#endif //PARSERLIB_AST_HPP
