#ifndef PARSERLIB_CORE_DEBUGPARSER_HPP
#define PARSERLIB_CORE_DEBUGPARSER_HPP


#include "TerminalParser.hpp"
#include "TerminalStringParser.hpp"


namespace parserlib::core {


    /**
     * DEBUG parser.
     *
     * Can be used to set a breakpoint at a specific point in a grammar.
     *
     * @param Child child parser.
     */
    template <class Child>
    class DebugParser : public Parser<DebugParser<Child>> {
    public:
        /**
         * The constructor.
         * @param child the child parser.
         */
        DebugParser(const Child& child)
            : m_child(child)
        {
        }

        /**
         * Parses the input.
         * @param pc parse context.
         * @return whatever the child parser returns.
         */
        template <class ParseContext>
        bool parse(ParseContext& pc) const {
            bool result = m_child.parse(pc);
            return result;
        }

    private:
        Child m_child;
    };


    /**
     * Adds a debug parser for the given parser.
     * @param child parser to add a debug parser to.
     * @return the debug parser.
     */
    template <class Child>
    DebugParser<Child> debug(const Parser<Child>& child) {
        return DebugParser<Child>(child.self());
    }


    /**
     * Adds a debug parser for the given terminal.
     * @param value value of terminal parser.
     * @return the debug parser.
     */
    template <class Value, std::enable_if_t<!std::is_base_of_v<Parser<Value>, Value>, bool> = true>
    DebugParser<TerminalParser<Value>> debug(const Value& value) {
        return DebugParser<TerminalParser<Value>>(TerminalParser<Value>(value));
    }


    /**
     * Adds a debug parser for the given terminal string.
     * @param value null-terminated terminal string.
     * @return the debug parser.
     */
    template <class Value>
    DebugParser<TerminalStringParser<Value>> debug(const Value* value) {
        return DebugParser<TerminalStringParser<Value>>(TerminalStringParser<Value>(value));
    }


} //namespace parserlib::core


#endif //PARSERLIB_CORE_DEBUGPARSER_HPP
