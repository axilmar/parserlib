#ifndef PARSERLIB_ASTNODE_HPP
#define PARSERLIB_ASTNODE_HPP


#include <memory>
#include <vector>


namespace parserlib
{


    /**
        Base class for AST nodes.
     */
    class ASTNode
    {
    public:
        /**
            Destructor.
         */
        virtual ~ASTNode()
        {
        }
    };


    ///pointer to ast node.
    typedef std::shared_ptr<ASTNode> ASTNodePtr;


    ///stack of ast nodes.
    typedef std::vector<ASTNodePtr> ASTNodeStack;


} //namespace parserlib


#endif //PARSERLIB_ASTNODE_HPP
