#ifndef PARSERLIB_CFE_EBNF_HPP
#define PARSERLIB_CFE_EBNF_HPP


#include "CFE.hpp"


namespace parserlib {


    /**
     * Grammar taken from:
     * 
     * https://en.wikipedia.org/wiki/Extended_Backus%E2%80%93Naur_form
     */
    template <class SourceType = std::string>
    class CFE_EBNF {
    public:
        enum class TokenType {
            String,
            Char,
            Identifier,
            Terminator,
            Assign,
            Pipe,
            Comma,
            QuestionMark,
            Star,
            Plus,
            Minus,
            OpenParen,
            CloseParen,
            OpenSquareBracket,
            CloseSquareBracket,
            OpenCurlyBracket,
            CloseCurlyBracket,
            Range
        };

        template <class T>
        friend std::basic_ostream<T, std::char_traits<T>>& operator << (std::basic_ostream<T, std::char_traits<T>>& stream, TokenType tokenType) {
            switch (tokenType) {
                case TokenType::String:
                    stream << "String";
                    break;
                case TokenType::Char:
                    stream << "Char";
                    break;
                case TokenType::Identifier:
                    stream << "Identifier";
                    break;
                case TokenType::Terminator:
                    stream << "Terminator";
                    break;
                case TokenType::Assign:
                    stream << "Assign";
                    break;
                case TokenType::Pipe:
                    stream << "Pipe";
                    break;
                case TokenType::Comma:
                    stream << "Comma";
                    break;
                case TokenType::QuestionMark:
                    stream << "QuestionMark";
                    break;
                case TokenType::Star:
                    stream << "Star";
                    break;
                case TokenType::Plus:
                    stream << "Plus";
                    break;
                case TokenType::Minus:
                    stream << "Minus";
                    break;
                case TokenType::OpenParen:
                    stream << "OpenParen";
                    break;
                case TokenType::CloseParen:
                    stream << "CloseParen";
                    break;
                case TokenType::OpenSquareBracket:
                    stream << "OpenSquareBracket";
                    break;
                case TokenType::CloseSquareBracket:
                    stream << "CloseSquareBracket";
                    break;
                case TokenType::OpenCurlyBracket:
                    stream << "OpenCurlyBracket";
                    break;
                case TokenType::CloseCurlyBracket:
                    stream << "CloseCurlyBracket";
                    break;
                case TokenType::Range:
                    stream << "Range";
                    break;
            }
            return stream;
        }

        enum class ASTType {
            RuleList,
            Rule,
            RuleName,
            Alternation,
            Concatenation,
            Optional,
            ZeroOrMore,
            OneOrMore,
            Exclusion,
            OptionalTerm,
            RepetitionTerm,
            String,
            CharRange
        };

        template <class T>
        friend std::basic_ostream<T, std::char_traits<T>>& operator << (std::basic_ostream<T, std::char_traits<T>>& stream, ASTType astType) {
            switch (astType) {
                case ASTType::RuleList:
                    stream << "RuleList";
                    break;
                case ASTType::Rule:
                    stream << "Rule";
                    break;
                case ASTType::RuleName:
                    stream << "RuleName";
                    break;
                case ASTType::Alternation:
                    stream << "Alternation";
                    break;
                case ASTType::Concatenation:
                    stream << "Concatenation";
                    break;
                case ASTType::Optional:
                    stream << "Optional";
                    break;
                case ASTType::ZeroOrMore:
                    stream << "ZeroOrMore";
                    break;
                case ASTType::OneOrMore:
                    stream << "OneOrMore";
                    break;
                case ASTType::Exclusion:
                    stream << "Exclusion";
                    break;
                case ASTType::OptionalTerm:
                    stream << "OptionalTerm";
                    break;
                case ASTType::RepetitionTerm:
                    stream << "RepetitionTerm";
                    break;
                case ASTType::String:
                    stream << "String";
                    break;
                case ASTType::CharRange:
                    stream << "CharRange";
                    break;
            }
            return stream;
        }

        using CFEType = CFE<TokenType, ASTType>;
        
        using LexerRuleType = typename CFEType::LexerRuleType;

        using RuleType = typename CFEType::RuleType;

        CFE_EBNF() {
            /**** the lexer ****/

            const auto commentStart = terminal("(*");
            const auto commentEnd = terminal("*)");

            auto comments = commentStart >> *(any - commentEnd) >> commentEnd;

            auto S = comments | terminalSet(" \n\t\r\f\b");

            auto letter = terminalRange('a', 'z') | terminalRange('A', 'Z');

            auto digit = terminalRange('0', '9');

            auto identifier = (letter >> *(letter | digit | '_')) == TokenType::Identifier;

            auto escapedChar = terminal('\\') >> terminalSet("ntrfb\'\"");

            auto symbol = terminalSet("[]{}()<>=|.,;-+*?");

            auto character = letter | digit | escapedChar | symbol | '_' | ' ';

            auto string = (terminal("'") >> (character - terminal("'")) >> +(character - terminal("'")) >> terminal("'")) == TokenType::String
                        | (terminal('"') >> (character - terminal('"')) >> +(character - terminal('"')) >> terminal('"')) == TokenType::String;

            auto char_ = (terminal("'") >> (character - terminal("'")) >> terminal("'")) == TokenType::Char
                       | (terminal('"') >> (character - terminal('"')) >> terminal('"')) == TokenType::Char;

            auto symbolToken 
                = terminal("::=") == TokenType::Assign
                | terminal("..") == TokenType::Range
                | terminal(';') == TokenType::Terminator
                | terminal('.') == TokenType::Terminator
                | terminal('=') == TokenType::Assign
                | terminal(':') == TokenType::Assign
                | terminal('|') == TokenType::Pipe
                | terminal(',') == TokenType::Comma
                | terminal('?') == TokenType::QuestionMark
                | terminal('*') == TokenType::Star
                | terminal('+') == TokenType::Plus
                | terminal('-') == TokenType::Minus
                | terminal('(') == TokenType::OpenParen
                | terminal(')') == TokenType::CloseParen
                | terminal('[') == TokenType::OpenSquareBracket
                | terminal(']') == TokenType::CloseSquareBracket
                | terminal('{') == TokenType::OpenCurlyBracket
                | terminal('}') == TokenType::CloseCurlyBracket;

            m_lexerGrammar = *(S | char_ | string | identifier | symbolToken);

            /**** the parser ****/

            //rule name
            auto ruleName = terminal(TokenType::Identifier) >= ASTType::RuleName;

            //term
            auto term = terminal(TokenType::OpenParen) >> m_alternation >> terminal(TokenType::CloseParen)
                      | (terminal(TokenType::OpenSquareBracket) >> m_alternation >> terminal(TokenType::CloseSquareBracket)) >= ASTType::OptionalTerm
                      | (terminal(TokenType::OpenCurlyBracket) >> m_alternation >> terminal(TokenType::CloseCurlyBracket)) >= ASTType::RepetitionTerm
                      | terminal(TokenType::String) >= ASTType::String
                      | ruleName;

            //optional
            auto optional = (term >> terminal(TokenType::QuestionMark)) >= ASTType::Optional;

            //zero or more
            auto zeroOrMore = (term >> terminal(TokenType::Star)) >= ASTType::ZeroOrMore;


            //one or more
            auto oneOrMore = (term >> terminal(TokenType::Plus)) >= ASTType::OneOrMore;

            //exclusion
            auto exclusion = ((term >> terminal(TokenType::Minus)) >> term) >= ASTType::Exclusion;

            //term range
            auto termRange = (terminal(TokenType::Char) >> terminal(TokenType::Range) >> TokenType::Char) >= ASTType::CharRange;

            //factor
            auto factor = optional | zeroOrMore | oneOrMore | exclusion | termRange | term;

            //concatenation
            auto concatenation = (factor >> +(-terminal(TokenType::Comma) >> factor)) >= ASTType::Concatenation
                               | factor;

            //alternation
            m_alternation = (concatenation >> +(terminal(TokenType::Pipe) >> concatenation)) >= ASTType::Alternation
                          | concatenation;
            
            //rule
            auto rule = (ruleName >> terminal(TokenType::Assign) >> m_alternation >> terminal(TokenType::Terminator)) >= ASTType::Rule;

            //grammar
            m_parserGrammar = (*rule) >= ASTType::RuleList;
        }

        std::tuple<bool, typename CFEType::ASTNodeContainerType, typename CFEType::ErrorContainerType> parse(const SourceType& input) {
            return m_cfe.parse(input, m_lexerGrammar, m_parserGrammar);
        }

    private:
        //the CFE to use
        CFEType m_cfe;

        //rules need to be kept around
        LexerRuleType m_lexerGrammar;
        RuleType m_alternation;
        RuleType m_parserGrammar;
    };


} //namespace parserlib


#endif //PARSERLIB_CFE_EBNF_HPP
