#include <cassert>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include "parserlib.hpp"
#include "calculator.hpp"


using namespace parserlib;
using namespace tests;


template <class T> double eval_ast_node(const T& node_ptr) {
    switch (node_ptr->id()) {
        case calculator::NUM: {
            std::stringstream stream;
            stream << node_ptr->source();
            double v{};
            stream >> v;
            return v;
        }

        case calculator::ADD:
            return eval_ast_node(node_ptr->children().front()) + eval_ast_node(node_ptr->children().back());

        case calculator::SUB:
            return eval_ast_node(node_ptr->children().front()) - eval_ast_node(node_ptr->children().back());

        case calculator::MUL:
            return eval_ast_node(node_ptr->children().front()) * eval_ast_node(node_ptr->children().back());

        case calculator::DIV:
            return eval_ast_node(node_ptr->children().front()) / eval_ast_node(node_ptr->children().back());
    }

    throw std::logic_error("unreachable code");
}


static void test_library_ast_node() {
    calculator calc;
    std::string source = "2.0+1.0-3.0*4.0/5.0";
    const double result = 2.0 + 1.0 - 3.0 * 4.0 / 5.0;
    parse_context<> context(source);
    assert(calc.grammar.parse(context));
    assert(context.is_end_parse_position());
    assert(context.matches().size() == 1);
    assert(calc.evaluate(context.matches()[0]) == result);
    ast_node_ptr_type<parse_definitions<>> ast_tree = create_ast_node(context.matches()[0]);
    assert(eval_ast_node(ast_tree) == result);
}


template <class ParseDefinitions>
class calculator_ast_node : public ast_node<ParseDefinitions> {
public:
    virtual double evaluate() const = 0;
};


template <class ParseDefinitions>
class num_ast_node : public calculator_ast_node<ParseDefinitions> {
public:
    num_ast_node(double value) : m_value(value) {
    }

    virtual double evaluate() const {
        return m_value;
    }

private:
    double m_value;
};


template <class ParseDefinitions>
class binary_ast_node : public calculator_ast_node<ParseDefinitions> {
public:
    binary_ast_node() {
    }

protected:
    std::shared_ptr<calculator_ast_node<ParseDefinitions>> left() const {
        return std::static_pointer_cast<calculator_ast_node<ParseDefinitions>>(this->children().front());
    }

    std::shared_ptr<calculator_ast_node<ParseDefinitions>> right() const {
        return std::static_pointer_cast<calculator_ast_node<ParseDefinitions>>(this->children().back());
    }
};


template <class ParseDefinitions>
class add_ast_node : public binary_ast_node<ParseDefinitions> {
public:
    using binary_ast_node<ParseDefinitions>::binary_ast_node;

    virtual double evaluate() const {
        return this->left()->evaluate() + this->right()->evaluate();
    }
};


template <class ParseDefinitions>
class sub_ast_node : public binary_ast_node<ParseDefinitions> {
public:
    using binary_ast_node<ParseDefinitions>::binary_ast_node;

    virtual double evaluate() const {
        return this->left()->evaluate() - this->right()->evaluate();
    }
};


template <class ParseDefinitions>
class mul_ast_node : public binary_ast_node<ParseDefinitions> {
public:
    using binary_ast_node<ParseDefinitions>::binary_ast_node;

    virtual double evaluate() const {
        return this->left()->evaluate() * this->right()->evaluate();
    }
};


template <class ParseDefinitions>
class div_ast_node : public binary_ast_node<ParseDefinitions> {
public:
    using binary_ast_node<ParseDefinitions>::binary_ast_node;

    virtual double evaluate() const {
        return this->left()->evaluate() / this->right()->evaluate();
    }
};


class custom_ast_node_factory {
public:
    template <class ParseDefinitions>
    ast_node_ptr_type<ParseDefinitions> operator ()(const match<ParseDefinitions>& m) const {
        switch (m.id()) {
            case calculator::NUM:
            {
                std::stringstream stream;
                stream << m.source();
                double d{};
                stream >> d;
                return std::make_shared<num_ast_node<ParseDefinitions>>(d);
            }

            case calculator::ADD:
                return std::make_shared<add_ast_node<ParseDefinitions>>();

            case calculator::SUB:
                return std::make_shared<sub_ast_node<ParseDefinitions>>();

            case calculator::MUL:
                return std::make_shared<mul_ast_node<ParseDefinitions>>();

            case calculator::DIV:
                return std::make_shared<div_ast_node<ParseDefinitions>>();
        }

        throw std::logic_error("unreachable code");
    }
};


static void test_custom_ast_node() {
    calculator calc;
    std::string source = "2.0+1.0-3.0*4.0/5.0";
    const double result = 2.0 + 1.0 - 3.0 * 4.0 / 5.0;
    parse_context<> context(source);
    assert(calc.grammar.parse(context));
    assert(context.is_end_parse_position());
    assert(context.matches().size() == 1);
    assert(calc.evaluate(context.matches()[0]) == result);
    ast_node_ptr_type<parse_definitions<>> ast_tree = create_ast_node(context.matches()[0], custom_ast_node_factory());
    assert(std::static_pointer_cast<calculator_ast_node<parse_definitions<>>>(ast_tree)->evaluate() == result);
}


void test_ast_node() {
    test_library_ast_node();
    test_custom_ast_node();
}
