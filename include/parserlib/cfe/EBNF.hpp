#ifndef PARSERLIB_CFE_EBNF_HPP
#define PARSERLIB_CFE_EBNF_HPP


#include <stdexcept>
#include <limits>
#include <string>
#include <ostream>
#include <set>
#include "parserlib/core/TerminalSetParser.hpp"
#include "parserlib/util.hpp"
#include "CFE.hpp"


namespace parserlib::cfe {


    using namespace core;


    /**
     * A CFE for EBNF (Extended Backus-Naur Form).
     * 
     * This class allows:
     *
     *  - the writing of tokenizer and parser grammars as EBNF in the same file.
     *  - the extraction of an AST (Abstract Syntax Tree) from an EBNF grammar.
     *  - the conversion of the extracted AST to a CFE class that allows parsing of said grammar programmatically.
     * 
     * The EBNF syntax used is mostly compatible with ISO EBNF, but it also has some important differences,
     * required for implementing this class.
     *
     * EBNF syntax supported:
     * 
     * - tokens:
     *      %<token name> = <choice expression> ';'
     *      <Token name> is an identifier.
     *      e.g.
     *      %integer = digit+;
     * 
     *      Explicitly defined tokens are automatically added to the tokenizer.
     * 
     *      Implicitly defined tokens (i.e. strings within the grammar) are extracted from the grammar
     *      and are added to the tokenizer.
     * 
     *      A rule can be defined either for the tokenizer and/or the parser, depending on use.
     *      If a rule is used within the context of a token, then it will be added to the tokenizer;
     *      if a rule is used within the context of a grammar rule, then it will be added to the parser;
     *      if a rule is used by both, it will be added to both.
     * 
     * - rules:
     *      <rule name> = <choice expression> ';'
     *      Rules are used both for the tokenizer and the parser.
     *      <Rule name> is an identifier.
     *      e.g.
     *      structDecl = 'struct' identifier '{' structMemberDecl* '}';
     * 
     * - operations:
     *      - assignment operator:
     *          Tokens and rules can optionally be defined by using operator '::=' or operator ':'. 
     *          e.g.
     *          rule1 ::= rule2 rule3;
     *          rule1 : rule2 rule3;
     *
     *      - prefix/postfix operator 'optional':
     *          <unary expression> '?'
     *          e.g.
     *          (a | b | c)?
     *          ?(a | b | c)
     *
     *      - prefix/postfix operator 'zero or more times':
     *          <unary expression> '*'
     *          e.g.
     *          (a | b | c)*
     *          *(a | b | c)
     *
     *      - prefix/postfix operator 'one or more times':
     *          <unary expression> '+'
     *          e.g.
     *          (a | b | c)+
     *          +(a | b | c)
     *
     *      - prefix/postfix operator 'logical AND':
     *          <unary expression> '&'
     *          e.g.
     *          (a | b | c)&
     *          &(a | b | c)
     *
     *      - prefix/postfix operator 'logical NOT':
     *          <unary expression> '!'
     *          e.g.
     *          (a | b | c)!
     *          !(a | b | c)
     *
     *      - infix operator 'exclude':
     *          <unary expression> '-' <unary expression>
     *          e.g.
     *          a - b
     *
     *      - infix operator 'choice':
     *          <sequence expression> '|' <sequence expression>
     *          e.g.
     *          a | b
     *
     *      - infix operator 'sequence':
     *          <unary expression> [','] <unary expression>
     *          The comma is optional.
     *          e.g.
     *          a b c
     * 
     * - grouping:
     *      - group:
     *          '(' <choice expression> ')'
     *          e.g.
     *          (a | b | c)
     *
     *      - Optional group:
     *          '[' <choice expression> ']'
     *          e.g.
     *          [a | b | c]
     *
     *      - Repeat zero or more times group:
     *          '{' <choice expression> '}'
     *          e.g.
     *          {a | b | c}
     * 
     *  An unary expression can be:
     *      - a group.
     *      - an identifier.
     *      - a terminal.
     * 
     *  identifiers:
     *      -an identifier can start with a letter ('a' to 'z' or 'A to 'Z') or the character '_'.
     *      -an identifier can continue with a sequence of letters, digit, dash ('-') or underscore ('_').
     *      e.g.
     *      declaration = declaration-1 | declaration-2 | type-modifier | _other_Identifier_2;
     *
     *  terminals:
     *      - string terminal:
     *          '"' <character sequence> '"' or
     *          "'" <character sequence> "'"
     * 
     *          A string can be enclosed either by double quotes
     *          or by single quotes.
     * 
     *          A string within double quotes cannot contain a double quote symbol;
     *          the double quote symbol must be escaped by '\'.
     * 
     *          A string within single quotes cannot contain a single quote symbol;
     *          the single quote symbol must be escaped by '\'.
     * 
     *          e.g.
     *          "the quick brown fox jumps over the \"lazy\" dog".
     *          'the quick brown fox jumps over the \'lazy\' dog'.
     *
     *      - char terminal:
     *          "'" <character> "'"
     * 
     *          The single quote symbol must be escaped.
     *          e.g.
     *          'a'
     *          '\''
     *          
     *      - characters:
     *          - any character enclosed in single or double quotes.
     *          - the special escaped characters \0, \n, \r, \t, \v, \", \'.
     *          - unicode characters in the form of \u<hex>; 
     *            e.g. \u0A, \uFF, \u01234ABCD.
     *          - any character: the underscore symbol '_';
     *            e.g. 
     *            (* all characters except 'a')
     *            _ - 'a'.
     * 
     * - comments:
     *      - comment start: (*
     *      - comment end: *)
     *      - comments are multiline.
     * 
     * Example of a calculator grammar.
     * 
     *  %comment = '(*' (_ - '*)')* '*)';
     * 
     *  %whitespace = '\0' .. ' ';
     * 
     *  digit = '0' .. '9';
     * 
     *  %number = digit+ ['.' digit+];
     * 
     *  add = add '+' mul
     *      | add '-' mul
     *      | mul
     *      ;
     *
     *  mul = mul '*' val
     *      | mul '/' val
     *      | mul
     *      ;
     * 
     *  val = '(' add ')'
     *      | number
     *      ;
     *
     * @param Source_ the type of the source code; an STL-like container.
     */
    template <class Source_ = core::SourceString<>> class EBNF {
    public:
        /**
         * Token id.
         */
        enum class TokenID {
            /**
             * Token identifier.
             */
            TokenIdentifier,

            /**
             * Identifier.
             */
            Identifier,

            /**
             * String.
             */
            String,

            /**
             * Character range.
             */
            CharRange,

            /**
             * Character.
             */
            Char,

            /**
             * Assignment symbol.
             */
            Assignment,

             /**
             * Choice symbol.
             */
            Choice,

            /**
             * Exclude symbol.
             */
            Exclude,

            /**
             * Zero or more symbol.
             */
            ZeroOrMore,

            /**
             * One or more symbol.
             */
            OneOrMore,

            /**
             * Optional symbol.
             */
            Optional,

            /**
             * Logical and.
             */
            LogicalAnd,

            /**
             * Logical not.
             */
            LogicalNot,

            /**
             * Group start symbol.
             */
            GroupStart,

            /**
             * Group end symbol.
             */
            GroupEnd,

            /**
             * Optional start.
             */
            OptionalGroupStart,

            /**
             * Optional end.
             */
            OptionalGroupEnd,

            /**
             * Repetition start.
             */
            RepetitionGroupStart,

            /**
             * Repetition end.
             */
            RepetitionGroupEnd,

            /**
             * Terminator symbol.
             */
            Terminator,

            /**
             * Any character.
             */
             AnyChar,

            /**
             * Sequence separator.
             */
            SequenceSeparator
        };

        /**
         * AST node id.
         */
        enum class ASTID {
            /**
             * Terminal character.
             */
            TerminalChar,

            /**
             * Terminal string.
             */
            TerminalString,

            /**
             * Terminal range.
             */
            TerminalRange,

             /**
             * Non-terminal.
             */
            NonTerminal,

            /**
             * Sequence.
             */
            Sequence,

            /**
             * Choice.
             */
            Choice,

            /**
             * Zero or more.
             */
            ZeroOrMore,

            /**
             * One or more.
             */
            OneOrMore,

            /**
             * Optional.
             */
            Optional,

            /**
             * Logical And.
             */
            LogicalAnd,

            /**
             * Logical Not.
             */
            LogicalNot,

            /**
             * Exclude.
             */
            Exclude,

            /**
             * Group.
             */
            Group,

            /**
             * Optional group.
             */
            OptionalGroup,

            /**
             * Repetition group.
             */
            RepetitionGroup,

            /**
             * Token
             */
            Token,

            /**
             * Rule.
             */
            Rule,

            /**
             * Any character.
             */
            AnyChar
        };

        /**
         * Prints the name of an AST id member.
         * @param stream output stream.
         * @param id ast id to print.
         * @return the output stream.
         */
        template <class CharT, class CharTraits>
        friend std::basic_ostream<CharT, CharTraits>& operator << (std::basic_ostream<CharT, CharTraits>& stream, ASTID id) {
            switch (id) {
                case ASTID::TerminalChar:
                    stream << "TerminalChar";
                    break;

                case ASTID::TerminalString:
                    stream << "TerminalString";
                    break;

                case ASTID::TerminalRange:
                    stream << "TerminalRange";
                    break;

                case ASTID::NonTerminal:
                    stream << "NonTerminal";
                    break;

                case ASTID::Sequence:
                    stream << "Sequence";
                    break;

                case ASTID::Choice:
                    stream << "Choice";
                    break;

                case ASTID::ZeroOrMore:
                    stream << "ZeroOrMore";
                    break;

                case ASTID::OneOrMore:
                    stream << "OneOrMore";
                    break;

                case ASTID::Optional:
                    stream << "Optional";
                    break;

                case ASTID::LogicalAnd:
                    stream << "LogicalAnd";
                    break;

                case ASTID::LogicalNot:
                    stream << "LogicalNot";
                    break;

                case ASTID::Exclude:
                    stream << "Exclude";
                    break;

                case ASTID::Group:
                    stream << "Group";
                    break;

                case ASTID::OptionalGroup:
                    stream << "OptionalGroup";
                    break;

                case ASTID::RepetitionGroup:
                    stream << "RepetitionGroup";
                    break;

                case ASTID::Token:
                    stream << "Token";
                    break;

                case ASTID::Rule:
                    stream << "Rule";
                    break;

                case ASTID::AnyChar:
                    stream << "AnyChar";
                    break;

                default:
                    throw std::logic_error(toString("Invalid AST id: ", id));
            }
            return stream;
        }

        /**
         * Type of source.
         */
        typedef typename Source_ Source;

        /**
         * Type of character.
         */
        typedef typename Source::value_type Char;

        /**
         * Type of CFE.
         */
        typedef CFE<TokenID, ASTID, Source> CFE;

        /**
         * Type of ptr to token container.
         */
        typedef typename CFE::TokenContainerPtr TokenContainerPtr;

        /**
         * Token container type.
         */
        typedef typename TokenContainerPtr::element_type TokenContainer;

        /**
         * Token iterator type.
         */
        typedef typename TokenContainer::const_iterator TokenIterator;

        /**
         * Token type.
         */
        typedef typename TokenContainer::value_type Token;

        /**
         * Type of rule for tokenization.
         */
        typedef typename CFE::TokenizerRule TokenizerRule;

        /**
         * Type of parser rule.
         */
        typedef typename CFE::ParserRule ParserRule;

        /**
         * Type of ptr to AST.
         */
        typedef typename CFE::ASTPtr ASTPtr;

        /**
         * AST type.
         */
        typedef typename ASTPtr::element_type AST;

        /**
         * Source view.
         */
        typedef typename AST::String String;

        /**
         * Char type.
         */
        typedef typename String::value_type Char;

        /**
         * Source view type.
         */
        typedef typename AST::StringView StringView;

        /**
         * Type of ptr to AST container.
         */
        typedef typename CFE::ASTContainerPtr ASTContainerPtr;

        /**
         * AST container.
         */
        typedef typename ASTContainerPtr::element_type ASTContainer;

        /**
         * Type of ptr to error container.
         */
        typedef typename CFE::ErrorContainerPtr ErrorContainerPtr;

        /**
         * Constructs an EBNF CFE.
         */
        EBNF() {
            createTokenizerGrammar();
            createParserGrammar();
        }

        /**
         * Parses an input.
         * @return success flag, tokens, ast nodes, errors.
         */
        std::tuple<bool, TokenContainerPtr, ASTContainerPtr, ErrorContainerPtr> parse(Source& input) {
            return CFE::parse(input, m_tokenizerGrammar, m_parserGrammar);
        }

        /**
         * Given a parsed EBNF, it creates a CFE file for that EBNF.
         * @param stream output stream.
         * @param includeGuard include guard string.
         * @param classNamespace namespace of the CFE class; it can be empty.
         * @param className name of the CFE class.
         * @param ast abstract-syntax-tree to convert to CFE.
         * @param separatorLineCount number of lines between parts of the file.
         * @param tabSize number of spaces per tab.
         * @exception std::invalid_argument thrown if:
         *  - no root rule exists.
         *  - multiple root rules exist.
         */
        template <class Char, class CharTraits, class StringAlloc>
        static void createCFE(
            std::basic_ostream<Char, CharTraits>& stream, 
            const std::basic_string<Char, CharTraits, StringAlloc>& includeGuard, 
            const std::basic_string<Char, CharTraits, StringAlloc>& classNamespace, 
            const std::basic_string<Char, CharTraits, StringAlloc>& className, 
            const ASTContainer& ast, 
            const std::vector<std::basic_string<Char, CharTraits, StringAlloc>>& additionalIncludes = {}, 
            size_t separatorLineCount = 2, 
            size_t tabSize = 4)
        {
            ASTContainer tokens, tokenizerRules, parserRules;
            ASTPtr rootParserRule = processAST(ast, tokens, tokenizerRules, parserRules);

            //add include guard
            stream << "#ifndef " << includeGuard << '\n';
            stream << "#define " << includeGuard << '\n';

            //add lines between guard and includes
            addEmptyLines(stream, separatorLineCount);

            //add includes
            for (const auto& include : additionalIncludes) {
                stream << toString("#include \"", include, '"');
            }
            stream << "#include \"parserlib/cfe/CFE.hpp\"\n";

            //add lines between includes and namespace
            addEmptyLines(stream, separatorLineCount);

            //open namespace
            const bool namespaceIsEmpty = classNamespace.empty();
            if (!namespaceIsEmpty) {
                stream << "namespace " << classNamespace << " {\n";
                addEmptyLines(stream, separatorLineCount);
            }

            //begin class
            addSpaces(stream, tabSize);
            stream << "template <class Source = core::SourceString<>>\n";
            addSpaces(stream, tabSize);
            stream << "class " << className << " {\n";

            //end class
            addSpaces(stream, tabSize);
            stream << "};\n";

            //add lines between class end and namespace
            addEmptyLines(stream, separatorLineCount);

            //close namespace
            if (!namespaceIsEmpty) {
                stream << "}\n";
                addEmptyLines(stream, separatorLineCount);
            }

            //close the include guard
            stream << "#endif //" << includeGuard << '\n';
        }

        template <class Char, class CharTraits>
        static void createCFE(
            std::basic_ostream<Char, CharTraits>& stream,
            const Char* includeGuard,
            const Char* classNamespace,
            const Char* className,
            const ASTContainer& ast,
            const std::vector<std::string>& additionalIncludes = {},
            size_t separatorLineCount = 2,
            size_t tabSize = 4)
        {
            createCFE(stream, std::basic_string<Char>(includeGuard), std::basic_string<Char>(classNamespace), std::basic_string<Char>(className), ast, additionalIncludes, separatorLineCount, tabSize);
        }

    private:
        //internal node that holds a specific value as source.
        class ASTValue : public AST {
        public:
            ASTValue(ASTID id, const TokenIterator& startPosition, const TokenIterator& endPosition, const String& value)
                : AST(id, startPosition, endPosition)
                , m_value(value)
            {
            }

            StringView getSource(size_t maxChars = -1) const override {
                return StringView(m_value.data(), m_value.size());
            }

        private:
            String m_value;
        };

        TokenizerRule m_tokenizerGrammar;
        ParserRule m_choice;
        ParserRule m_parserGrammar;

        void createTokenizerGrammar() {
            //any char
            const Char minChar = std::numeric_limits<Char>::min();
            const Char maxChar = std::numeric_limits<Char>::max();
            const auto anyChar = oneIn(minChar, maxChar);

            //whitespace
            const auto ws = oneIn('\0', ' ');

            //comment
            const auto commentStart = term("(*");
            const auto commentEnd = term("*)");
            const auto commentChar = anyChar - commentEnd;
            const auto comment = commentStart >> *commentChar >> commentEnd;

            const auto letter = oneIn('a', 'z') | oneIn('A', 'Z');
            const auto digit = oneIn('0', '9');

            const auto identifierChar = letter | digit | '-' | '_';
            const auto identifierLiteral = (letter | ('_' >> identifierChar)) >> *identifierChar;

            //token
            const auto tokenIdentifier = '%' >> (identifierLiteral ->* TokenID::TokenIdentifier);

            //identifier
            const auto identifier = identifierLiteral ->* TokenID::Identifier;

            //escaped characters
            auto escapedChar
                = term("\\0")
                | term("\\n")
                | term("\\t")
                | term("\\r")
                | term("\\v")
                | term("\\\"")
                | term("\\\'")
                ;

            //unicode character
            const auto hexDigit = oneOf("0123456789abcdefABCDEF");
            const auto unicodeChar = term('\\') >> (term('u') | term('U')) >> +hexDigit;

            //char
            const auto charStart = term('\'');
            const auto charEnd = term('\'');
            const auto charCharLiteral = escapedChar | unicodeChar | (anyChar - '\'');
            const auto char_ = charStart >> (charCharLiteral ->* TokenID::Char) >> charEnd;

            //string
            const auto stringStart = term('\"');
            const auto stringEnd = term('\"');
            const auto stringCharLiteral = escapedChar | unicodeChar | (anyChar - '"');
            const auto string 
                = stringStart >> (*stringCharLiteral->*TokenID::String) >> stringEnd
                | charStart >> ((charCharLiteral >> +charCharLiteral) ->* TokenID::String) >> charEnd;

            //char range
            const auto charRange = term("..") ->* TokenID::CharRange;

            //assignment symbol
            const auto assignment = (term("::=") | term('=') | term(':'))->*TokenID::Assignment;

            //other symbols
            const auto choice               = term('|') ->* TokenID::Choice;
            const auto exclude              = term('-') ->* TokenID::Exclude;
            const auto zeroOrMore           = term('*') ->* TokenID::ZeroOrMore;
            const auto oneOrMore            = term('+') ->* TokenID::OneOrMore;
            const auto optional             = term('?') ->* TokenID::Optional;
            const auto logicalAnd           = term('&') ->* TokenID::LogicalAnd;
            const auto logicalNot           = term('!') ->* TokenID::LogicalNot;
            const auto groupStart           = term('(') ->* TokenID::GroupStart;
            const auto groupEnd             = term(')') ->* TokenID::GroupEnd;
            const auto optionalGroupStart   = term('[') ->* TokenID::OptionalGroupStart;
            const auto optionalGroupEnd     = term(']') ->* TokenID::OptionalGroupEnd;
            const auto repetitionGroupStart = term('{') ->* TokenID::RepetitionGroupStart;
            const auto repetitionGroupEnd   = term('}') ->* TokenID::RepetitionGroupEnd;
            const auto terminator           = term(';') ->* TokenID::Terminator;
            const auto anyChar_             = term('_') ->* TokenID::AnyChar;
            const auto sequenceSeparator    = term(',') ->* TokenID::SequenceSeparator;

            //token
            const auto token 
                = ws
                | comment 
                | tokenIdentifier
                | identifier
                | string 
                | char_ 
                | charRange
                | assignment
                | choice
                | exclude
                | zeroOrMore
                | oneOrMore
                | optional
                | logicalAnd
                | logicalNot
                | groupStart
                | groupEnd
                | optionalGroupStart
                | optionalGroupEnd
                | repetitionGroupStart
                | repetitionGroupEnd
                | terminator
                | anyChar_
                | sequenceSeparator;

            //the tokenizer
            m_tokenizerGrammar = *(token);
        }

        void createParserGrammar() {
            const auto terminalString = TokenID::String ->* ASTID::TerminalString;

            const auto terminalChar = TokenID::Char ->* ASTID::TerminalChar;

            const auto terminalRange = (terminalChar >> TokenID::CharRange >> terminalChar) ->* ASTID::TerminalRange;

            const auto anyChar = TokenID::AnyChar ->* ASTID::AnyChar;

            const auto terminal 
                = terminalString
                | terminalRange
                | terminalChar
                | anyChar;

            const auto group = (TokenID::GroupStart >> m_choice >> TokenID::GroupEnd) ->* ASTID::Group;

            const auto optionalGroup = (TokenID::OptionalGroupStart >> m_choice >> TokenID::OptionalGroupEnd)->*ASTID::OptionalGroup;

            const auto repetitionGroup = (TokenID::RepetitionGroupStart >> m_choice >> TokenID::RepetitionGroupEnd)->*ASTID::RepetitionGroup;

            const auto tokenNonTerminal = TokenID::TokenIdentifier ->* ASTID::NonTerminal;

            const auto nonTerminal = TokenID::Identifier ->* ASTID::NonTerminal;

            const auto atom
                = group
                | optionalGroup
                | repetitionGroup
                | nonTerminal
                | terminal;

            const auto zeroOrMore = (atom >> TokenID::ZeroOrMore | TokenID::ZeroOrMore >> atom) ->* ASTID::ZeroOrMore;

            const auto oneOrMore = (atom >> TokenID::OneOrMore | TokenID::OneOrMore >> atom) ->* ASTID::OneOrMore;

            const auto optional = (atom >> TokenID::Optional | TokenID::Optional >> atom) ->* ASTID::Optional;

            const auto logicalNot = (atom >> TokenID::LogicalNot | TokenID::LogicalNot >> atom) ->* ASTID::LogicalNot;

            const auto logicalAnd = (atom >> TokenID::LogicalAnd | TokenID::LogicalAnd >> atom)->*ASTID::LogicalAnd;

            const auto unaryExpr
                = zeroOrMore
                | oneOrMore
                | optional
                | logicalAnd
                | logicalNot
                | atom;

            const auto exclude 
                = (unaryExpr >> TokenID::Exclude >> unaryExpr) ->* ASTID::Exclude
                | unaryExpr;

            const auto sequence 
                = (exclude >> +exclude) ->* ASTID::Sequence
                | exclude;

            m_choice 
                = (sequence >> (-term(TokenID::SequenceSeparator) >> +(TokenID::Choice >> sequence))) ->* ASTID::Choice
                | sequence;

            const auto token = (tokenNonTerminal >> TokenID::Assignment >> m_choice) ->* ASTID::Token;

            const auto rule = (nonTerminal >> TokenID::Assignment >> m_choice) ->* ASTID::Rule;

            const auto decl = (token | rule) >> ~term(TokenID::Terminator);

            m_parserGrammar = *decl;
        }


        static ASTPtr findRule(const ASTContainer& rules, const StringView& ruleName) {
            for (const ASTPtr& rule : rules) {
                if (rule->getChildren()[0]->getSource() == ruleName) {
                    return rule;
                }
            }
            return nullptr;
        }

        static bool referencesNode(
            const ASTPtr& node, 
            const ASTContainer& rules, 
            ASTID astID, 
            const StringView& source, 
            std::set<ASTPtr>& recursiveNodes)
        {
            //found node
            if (node->getID() == astID && node->getSource() == source) {
                return true;
            }

            //handle recursive rules
            auto [it, ok] = recursiveNodes.insert(node);
            if (!ok) {
                return false;
            }
            OnScopeExit restoreRecursiveNodes([&]() { recursiveNodes.erase(it); });

            //if found non-terminal, follow it via the rules
            if (node->getID() == ASTID::NonTerminal) {
                const ASTPtr rule = findRule(rules, node->getSource());
                if (!rule) {
                    return false;
                }
                return referencesNode(rule->getChildren()[1], rules, astID, source, recursiveNodes);
            }

            //check children
            for (const ASTPtr& child : node->getChildren()) {
                if (referencesNode(child, rules, astID, source, recursiveNodes)) {
                    return true;
                }
            }

            //not found
            return false;
        }

        static bool isTokenizerRule(const ASTContainer& tokens, const ASTContainer& rules, const StringView& ruleName) {
            for (const ASTPtr& token : tokens) {
                const ASTPtr& tokenExpr = token->getChildren()[1];
                std::set<ASTPtr> recursiveNodes;
                if (referencesNode(tokenExpr, rules, ASTID::NonTerminal, ruleName, recursiveNodes)) {
                    return true;
                }
            }
            return false;
        }

        static bool isParserRule(const ASTContainer& rules, const StringView& ruleName) {
            for (const ASTPtr& rule : rules) {
                const ASTPtr& ruleExpr = rule->getChildren()[1];
                std::set<ASTPtr> recursiveNodes;
                if (referencesNode(ruleExpr, rules, ASTID::NonTerminal, ruleName, recursiveNodes)) {
                    return true;
                }
            }
            return false;
        }

        static String getTokenName(const Char* chars, size_t size) {
            std::basic_stringstream<Char> stream;

            for (const Char* end = chars + size; chars < end; ++chars) {
                auto ch = *chars;
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || (ch >= '0' && ch <= '9') || ch == '-') {
                    stream << ch;
                }
                else {
                    stream << replaceChar(ch);
                }
            }

            return stream.str();
        }

        static void extractInlineTokens(const ASTPtr& node, ASTContainer& tokens, std::set<ASTPtr>& recursiveNodes) {
            switch (node->getID()) {
                case ASTID::TerminalChar:
                    return;

                case ASTID::TerminalRange:
                    return;

                case ASTID::TerminalString: {
                    const ASTPtr originalNode = node;
                    const ASTPtr parent = node->getParent();
                    StringView source = originalNode->getSource();
                    String tokenName = getTokenName(source.data(), source.size());

                    //replace the terminal string expression with a non-terminal that references the token
                    parent->replaceChild(originalNode, std::make_shared<ASTValue>(ASTID::NonTerminal, originalNode->getStartPosition(), originalNode->getEndPosition(), tokenName));

                    //create a token
                    ASTPtr token = std::make_shared<AST>(ASTID::Token, originalNode->getStartPosition(), originalNode->getEndPosition());

                    //tokens require the first child to be a non terminal
                    token->addChild(std::make_shared<ASTValue>(ASTID::NonTerminal, originalNode->getStartPosition(), originalNode->getEndPosition(), tokenName));

                    //the second child of the token is the terminal itself, which is now removed from its previous parent
                    token->addChild(originalNode);

                    //save the new token
                    tokens.push_back(token);
                    return;
                }
            }

            auto [it, ok] = recursiveNodes.insert(node);
            if (!ok) {
                return;
            }
            OnScopeExit restoreRecursiveNodes([&]() { recursiveNodes.erase(it); });

            for (const ASTPtr& child : node->getChildren()) {
                extractInlineTokens(child, tokens, recursiveNodes);
            }
        }

        static ASTPtr processAST(const ASTContainer& ast, ASTContainer& tokens, ASTContainer& tokenizerRules, ASTContainer& parserRules) {
            ASTContainer rules;

            //get explicit tokens and rules
            for (const ASTPtr& astNode : ast) {
                if (astNode->getID() == ASTID::Token) {
                    tokens.push_back(astNode);
                }
                else if (astNode->getID() == ASTID::Rule) {
                    rules.push_back(astNode);
                }
                else {
                    throw std::logic_error("Invalid EBNF AST node type.");
                }
            }

            //get the rules for the tokenizer and for the parser.
            //Also get the root rule.
            std::set<ASTPtr> rootRules;
            for (const ASTPtr& astNode : rules) {
                bool tokenizerRule = false, parserRule = false;
                if (isTokenizerRule(tokens, rules, astNode->getChildren()[0]->getSource())) {
                    tokenizerRule = true;
                    tokenizerRules.push_back(astNode);
                }
                if (isParserRule(rules, astNode->getChildren()[0]->getSource())) {
                    parserRule = true;
                    parserRules.push_back(astNode);
                }
                if (!tokenizerRule && !parserRule) {
                    rootRules.insert(astNode);
                }
            }

            //get the inline tokens
            std::set<ASTPtr> recursiveNodes;
            for (ASTPtr& parserRule : parserRules) {
                extractInlineTokens(parserRule->getChildren()[1], tokens, recursiveNodes);
            }

            //must have at least one root rule
            if (rootRules.size() == 0) {
                throw std::invalid_argument("EBNF: no root rule found.");
            }

            //must not have multiple root rules
            if (rootRules.size() > 1) {
                throw std::invalid_argument("EBNF: multiple root rules found.");
            }

            //returns the root rule
            return *rootRules.begin();
        }
    };


} //namespace parserlib::cfe


#endif //PARSERLIB_CFE_EBNF_HPP
