#ifndef PARSERLIB__EXAMPLES__CALCULATOR__CALCULATOR_AST__HPP
#define PARSERLIB__EXAMPLES__CALCULATOR__CALCULATOR_AST__HPP


#include "parserlib.hpp"


//base class for expressions
class ast_expr : public parserlib::ast_node<>
{
public:
	virtual double eval() const = 0;
};


//number
class ast_num : public ast_expr
{
public:
	template <typename Match>
	ast_num(const Match& m, parserlib::ast_node_stack<>& ast_nodes)
		: m_value(std::stod(m))
	{
	}

	virtual double eval() const
	{
		return m_value;
	}

private:
	const double m_value;
};


//base class for binary expressions
class ast_binary_expr : public ast_expr
{
public:
	template <typename Match>
	ast_binary_expr(const Match& m, parserlib::ast_node_stack<>& ast_nodes)
	{
		m_right = parserlib::pop_ast_node<ast_expr>(ast_nodes);
		m_left = parserlib::pop_ast_node<ast_expr>(ast_nodes);
	}

protected:
	std::shared_ptr<ast_expr> m_left, m_right;
};


//division
class ast_div : public ast_binary_expr
{
public:
	using ast_binary_expr::ast_binary_expr;

	virtual double eval() const
	{
		return m_left->eval() / m_right->eval();
	}
};


//multiplication
class ast_mul : public ast_binary_expr
{
public:
	using ast_binary_expr::ast_binary_expr;

	virtual double eval() const
	{
		return m_left->eval() * m_right->eval();
	}
};


//subtraction
class ast_sub : public ast_binary_expr
{
public:
	using ast_binary_expr::ast_binary_expr;

	virtual double eval() const
	{
		return m_left->eval() - m_right->eval();
	}
};


//addition
class ast_add : public ast_binary_expr
{
public:
	using ast_binary_expr::ast_binary_expr;

	virtual double eval() const
	{
		return m_left->eval() + m_right->eval();
	}
};


extern const parserlib::match_table<> calculator_match_table;


#endif //PARSERLIB__EXAMPLES__CALCULATOR__CALCULATOR_AST__HPP

