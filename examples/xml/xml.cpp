#include <iostream>
#include <list>
#include <sstream>
#include "parserlib.hpp"

using namespace std;
using namespace parserlib;


/*
    DISCLAIMER:
    This is not a full XML parser; it only provides basic XML support,
    in order to show the library's features and usage.
 */


namespace xml_example {


/******************************************************************************
    GRAMMAR
 ******************************************************************************/


//whitespace
auto whitespace = range(0, 32);


//letter
auto letter = range('a', 'z')
            | range('A', 'Z');


//digit
auto digit = range('0', '9');


//any character
auto any_char = range(0, 255);


//identifier
Rule<> identifier = letter >> *(letter | digit);


//attribute value
Rule<> attribute_value = *(!terminal('\"') >> any_char);


//attribute
Rule<> attribute = identifier >> *whitespace >> '=' >> *whitespace >> '\"' >> attribute_value >> '\"';


//opening tag
Rule<> opening_tag = '<' >> identifier >> *(+whitespace >> attribute) >> *whitespace >> '>';


//element content
Rule<> content = +(!oneOf("<\n") >> any_char);


//closing tag
Rule<> closing_tag = "</" >> identifier >> *whitespace >> '>';


//element
Rule<> element = *whitespace >> opening_tag >> *(*whitespace >> (element | content)) >> closing_tag >> *whitespace;


/******************************************************************************
    AST
 ******************************************************************************/


/* non-XML AST nodes */


//identifier
class Identifier : public ASTNode
{
public:
    Identifier(const Match<>& match, ASTNodeStack& ans)
        : m_value(match)
    {
    }

    const std::string& getValue() const
    {
        return m_value;
    }

private:
    std::string m_value;
};


//identifier ptr
typedef std::shared_ptr<Identifier> IdentifierPtr;


//attribute value
class AttributeValue : public ASTNode
{
public:
    AttributeValue(const Match<>& match, ASTNodeStack& ans)
        : m_value(match)
    {
    }

    const std::string& getValue() const
    {
        return m_value;
    }

private:
    std::string m_value;
};


//attribute value ptr
typedef std::shared_ptr<AttributeValue> AttributeValuePtr;


//attribute node
class Attribute : public ASTNode
{
public:
    Attribute(const Match<>& match, ASTNodeStack& ans)
    {
        AttributeValuePtr valuePtr = ans.pop<AttributeValue>();
        IdentifierPtr idPtr = ans.pop<Identifier>();
        m_name = idPtr->getValue();
        m_value = valuePtr->getValue();
    }

    void print(std::ostream& stream)
    {
        stream << ' ' << m_name << "=\"" << m_value << "\"";
    }

private:
    string m_name;
    string m_value;
};


//attribute pointer
typedef std::shared_ptr<Attribute> AttributePtr;


/* XML AST nodes */


//base class for xml nodes
class XMLNode : public ASTNode
{
public:
    virtual void print(std::ostream& stream, int depth = 0) = 0;
};


//xml node pointer type
typedef std::shared_ptr<XMLNode> XMLNodePtr;


//element node
class Element : public XMLNode
{
public:
    Element(const Match<>& match, ASTNodeStack& ans)
    {
        //take closing tag
        IdentifierPtr closingTagIdPtr = ans.pop<Identifier>();

        //take children
        while (true)
        {
            XMLNodePtr node = ans.pop<XMLNode>();
            if (!node) break;
            m_children.push_front(node);
        }

        //take attributes
        while (true)
        {
            AttributePtr attr = ans.pop<Attribute>();
            if (!attr) break;
            m_attributes.push_front(attr);
        }

        //take opening tag
        IdentifierPtr openingTagIdPtr = ans.pop<Identifier>();

        //check the opening tag against the closing tag
        if (openingTagIdPtr->getValue() != closingTagIdPtr->getValue())
        {
            throw std::logic_error("invalid element");
        }

        m_name = openingTagIdPtr->getValue();
    }

    virtual void print(std::ostream& stream, int depth = 0)
    {
        stream << std::string(depth * 4, ' ') << '<' << m_name;
        
        for (const AttributePtr& attr : m_attributes)
        {
            attr->print(stream);
        }
        
        stream << ">\n";

        for (const XMLNodePtr& child : m_children)
        {
            child->print(stream, depth + 1);
        }

        stream << std::string(depth * 4, ' ') << "</" << m_name << ">\n";
    }

private:
    std::string m_name;
    std::list<AttributePtr> m_attributes;
    std::list<XMLNodePtr> m_children;
};


//content node
class Content : public XMLNode
{
public:
    Content(const Match<>& match, ASTNodeStack& ans)
        : m_value(match)
    {
    }

    virtual void print(std::ostream& stream, int depth = 0)
    {
        stream << std::string(depth * 4, ' ') << m_value << "\n";
    }

private:
    std::string m_value;
};


/******************************************************************************
    GRAMMAR<->AST
 ******************************************************************************/


static AST<Identifier> ast_identifier(identifier);
static AST<AttributeValue> ast_attribute_value(attribute_value);
static AST<Attribute> ast_attribute(attribute);
static AST<Element> ast_element(element);
static AST<Content> ast_content(content);


/******************************************************************************
    main
 ******************************************************************************/


static void test()
{
    std::string input = R"(<root>
    <child0>
        <childA>
        </childA>
        <childB>
        </childB>
    </child0>
    <child1 id="2">
    </child1>
    <child2 name="foo">
        the quick brown fox
        <childC>
        </childC>
    </child2>
    jumps over the lazy dog
</root>
)";

    ParseContext<> parseContext(input);
    auto root = parseContext.parse<Element>(element);

    if (root)
    {
        stringstream stream;
        root->print(stream);
        const std::string output = stream.str();
        if (output == input)
        {
            cout << "SUCCESS\n";
        }
        else
        {
            cout << "ERROR: input=\n" << input << '\n' << "output=\n" << output << endl;
            throw std::runtime_error("XML example error");
        }
    }
    else
    {
        cout << "FAILURE:\n";
        cout << parseContext.getRemainingInput() << endl;
    }
}


} //namespace xml_example


void runXMLExampleTests()
{
    cout << "XML example - start\n";
    xml_example::test();
    cout << "XML example - end\n\n";
}
