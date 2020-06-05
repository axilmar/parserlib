#ifndef PARSERLIB__AST__HPP
#define PARSERLIB__AST__HPP


#include <functional>
#include <memory>
#include <vector>
#include <map>
#include <string_view>
#include "parse_context.hpp"


namespace parserlib
{


    /**
        Base class for AST nodes.
        @param ParseContext parse context type.
     */
    template <typename ParseContext = parse_context<>> 
    class ast_node
    {
    public:
        ///parse context type.
        typedef ParseContext parse_context_type;

        ///destructor.
        virtual ~ast_node()
        {
        }
    };


    ///type of ast node pointer.
    template <typename ParseContext = parse_context<>>
    using ast_node_ptr = std::shared_ptr<ast_node<ParseContext>>;


    ///type of ast node stack.
    template <typename ParseContext = parse_context<>>
    using ast_node_stack = std::vector<ast_node_ptr<ParseContext>>;


    /**
        Helper function for retrieving an ast node from an ast node stack.
        @param ans ast node stack.
        @return pointer at the back of the node stack.
        @exception std::invalid_argument thrown if the ast node stack is empty.
     */
    template <typename T, typename ParseContext> 
    std::shared_ptr<T> pop_ast_node(ast_node_stack<ParseContext>& ans)
    {
        if (ans.empty())
        {
            throw std::invalid_argument("empty ast node stack");
        }
        const auto ptr = ans.back();
        ans.pop_back();
        return std::dynamic_pointer_cast<T>(ptr);
    }


    ///ast node creation function.
    template <typename ParseContext>
    using ast_node_creation_function = std::function<ast_node_ptr<ParseContext>(const typename ParseContext::match&, ast_node_stack<ParseContext> &)>;


    /**
        Helper function for declaring ast node creation functions.
        @param T type of AST node to create.
        @return ast node creation function.
     */
    template <typename T> 
    ast_node_creation_function<typename T::parse_context_type> ast()
    {
        return [](const typename T::parse_context_type::match& m, ast_node_stack<T::parse_context_type> &ans)
        {
            return std::make_shared<T>(m, ans);
        };
    }


    ///type of match table.
    template <typename ParseContext>
    using match_table_type = std::map<std::string_view, ast_node_creation_function<ParseContext>>;


    /**
        Match table type.
        @param ParseContext parse context type.
     */
    template <typename ParseContext = parse_context<>>
    class match_table : public match_table_type<ParseContext>
    {
    public:
        using match_table_type<ParseContext>::match_table_type;

        /**
            Creates the ast nodes.
            @param matches matches coming from parsing.
            @return the root ast node.
            @exception std::runtime_error thrown if, at the end, the ast node stack does not have a single node in it.
         */
        ast_node_ptr<ParseContext> create_ast(const std::vector<typename ParseContext::match>& matches) const
        {
            ast_node_stack<ParseContext> node_stack;

            //process matches
            for (const auto& match : matches)
            {
                //find ast creation function by tag
                const auto it = match_table_type<ParseContext>::find(match.tag);            

                //execute the ast creation function
                if (it != match_table_type<ParseContext>::end())
                {
                    node_stack.push_back(it->second(match, node_stack));
                }
            }

            //if the node stack does not contain one element,
            //it means there is some parsing error
            if (node_stack.size() != 1)
            {
                throw std::runtime_error("invalid match");
            }

            //return the one and only node stack element
            return node_stack.back();
        }
    };


} //namespace parserlib


#endif //PARSERLIB__AST__HPP
