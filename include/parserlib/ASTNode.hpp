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
    class ASTNodeStack : public std::vector<ASTNodePtr>
    {
    public:
        using std::vector<ASTNodePtr>::vector;

        /**
            Checks if the top element is of the given class,
            and if so, it pops the top element and
            returns a shared pointer to it.
            If the top element is not of the given class,
            a null pointer is returned.
            @param T target class.
         */
        template <class T> std::shared_ptr<T> pop()
        {
            if (dynamic_cast<T *>(back().get()))
            {
                std::shared_ptr<T> result{std::dynamic_pointer_cast<T>(back())};
                pop_back();
                return std::move(result);
            }
            return nullptr;
        }
    };


} //namespace parserlib


#endif //PARSERLIB_ASTNODE_HPP
