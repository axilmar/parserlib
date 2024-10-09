#ifndef PARSERLIB_TESTS_UTIL_HPP
#define PARSERLIB_TESTS_UTIL_HPP


#include <string>
#include <memory>
#include <vector>
#include <utility>
#include "parserlib/cfe/AST.hpp"


class Tree {
public:
    Tree(std::string&& str)
        : m_text(std::move(str))
    {
    }

    Tree(std::string&& str, std::vector<std::unique_ptr<Tree>>&& children)
        : m_text(std::move(str))
        , m_children(std::move(children))
    {
    }

    const std::string& getText() const {
        return m_text;
    }

    const std::vector<std::unique_ptr<Tree>>& getChildren() const {
        return m_children;
    }

private:
    std::string m_text;
    std::vector<std::unique_ptr<Tree>> m_children;
};


template <class... Children>
inline std::unique_ptr<Tree> tree(std::string&& str, Children&&... children) {
    std::vector<std::unique_ptr<Tree>> childContainer;
    (childContainer.push_back(std::move(children)), ...);
    return std::make_unique<Tree>(std::move(str), std::move(childContainer));
}


inline std::unique_ptr<Tree> tree(std::string&& str) {
    return std::make_unique<Tree>(std::move(str));
}


template <class ASTID, class Source>
bool operator == (const std::shared_ptr<parserlib::cfe::AST<ASTID, Source>>& ast, const std::unique_ptr<Tree>& tree) {
    if (ast->toString() != tree->getText()) {
        return false;
    }

    if (ast->getChildren().size() != tree->getChildren().size()) {
        return false;
    }

    auto itAST = ast->getChildren().begin();
    auto itTree = tree->getChildren().begin();

    for (; itAST != ast->getChildren().end(); ++itAST, ++itTree) {
        if (!(*itAST == *itTree)) {
            return false;
        }
    }

    return true;
}


#endif //PARSERLIB_TESTS_UTIL_HPP
