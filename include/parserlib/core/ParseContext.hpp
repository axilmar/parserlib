#ifndef PARSERLIB_CORE_PARSECONTEXT_HPP
#define PARSERLIB_CORE_PARSECONTEXT_HPP


#include <vector>
#include <tuple>
#include <optional>
#include <map>
#include <algorithm>
#include <stdexcept>
#include "ParseState.hpp"
#include "ParseError.hpp"
#include "ParseErrorState.hpp"
#include "SourceString.hpp"
#include "LeftRecursion.hpp"
#include "Match.hpp"


namespace parserlib::core {


    template <class ParseContext> class Rule;


    /**
     * Data used while parsing an input.
     * 
     * @param MatchID the match id type; by default, it is of type int, so C-like enums can be used for matches.
     *  But it can also be a class enum or any other type.
     * 
     * @param Source the type of source to parse; by default, it is a SourceString, allowing the counting of
     *  lines and columns, but it can also be any other STL-like container.
     */
    template <class MatchID = int, class Source = SourceString<>>
    class ParseContext {
    public:
        /**
         * The Match id type.
         */
        typedef MatchID MatchID;

        /**
         * The source type.
         */
        typedef Source Source;

        /**
         * The iterator to use for traversing the source.
         */
        typedef typename Source::const_iterator Iterator;

        /**
         * The parse state.
         */
        typedef ParseState<Source> ParseState;

        /**
         * The parse error.
         */
        typedef ParseError<Source> ParseError;

        /**
         * The parse error container.
         */
        typedef std::vector<ParseError> ParseErrorContainer;

        /**
         * The rule type for this parse context.
         */
        typedef Rule<ParseContext<MatchID, Source>> Rule;

        /**
         * The match type.
         */
        typedef Match<MatchID, Source> Match;

        /**
         * The match container type.
         */
        typedef MatchContainer<MatchID, Source> MatchContainer;

        /**
         * The constructor.
         * @param src the source; it must not go out of scope while the parse context is in scope.
         */
        ParseContext(Source& src)
            : m_currentPosition(src.begin())
            , m_furthestUnparsedPosition(src.begin())
            , m_endPosition(src.end())
        {
        }

        /**
         * Returns the current parsing position.
         * @return the current parsing position.
         */
        const Iterator& getCurrentPosition() const {
            return m_currentPosition;
        }

        /**
         * Returns the source's end position.
         * @return the source's end position.
         */
        const Iterator& getEndPosition() const {
            return m_endPosition;
        }

        /**
         * Checks if the end position is reached, 
         * i.e. if current position equals the end position.
         * @return true if the end position has been reached, false otherwise.
         */
        bool isEndPosition() const {
            return m_currentPosition == m_endPosition;
        }

        /**
         * Increments the current position by one.
         * No check is performed if the source end has been reached,
         * due to this function called repeatedly by parsers.
         */
        void incrementPosition() {
            ++m_currentPosition;
            if (m_currentPosition > m_furthestUnparsedPosition) {
                m_furthestUnparsedPosition = m_currentPosition;
            }
        }

        /**
         * Increments the current position by the given size.
         * No check is performed if the source end has been reached,
         * due to this function called repeatedly by parsers.
         * @param size number of positions to increase the current position.
         */
        void incrementPosition(size_t size) {
            m_currentPosition += size;
            if (m_currentPosition > m_furthestUnparsedPosition) {
                m_furthestUnparsedPosition = m_currentPosition;
            }
        }

        /**
         * Returns the furthest position that parsing stopped at.
         * @return the furthest position that parsing stopped at.
         */
        const Iterator& getFurthestUnparsedPosition() const {
            return m_furthestUnparsedPosition;
        }

        /**
         * Returns the current parse state.
         * @return the current parse state.
         */
        ParseState getParseState() const {
            return ParseState(m_currentPosition, m_matches.size(), m_leftRecursionMatches.size());
        }

        /**
         * Sets the current parse state of the context.
         * @param state the parse state to set.
         */
        void setParseState(const ParseState& state) {
            m_currentPosition = state.getPosition();
            m_matches.resize(state.getMatchCount());
            m_leftRecursionMatches.resize(state.getLeftRecursionMatchStateCount());
        }

        /**
         * Returns the current set of errors.
         * @return the current set of errors.
         */
        const ParseErrorContainer& getErrors() const {
            return m_errors;
        }

        /**
         * Adds the error at the given position.
         * @param errorId error id.
         * @param startPosition start position.
         * @param endPosition end position.
         */
        template <class ErrorType>
        void addError(ErrorType errorId, const Iterator& startPosition, const Iterator& endPosition) {
            m_errors.push_back(ParseError(errorId, startPosition, endPosition));
        }

        /**
         * Returns the current parse error state of the context.
         * @return the current parse error state of the context.
         */
        ParseErrorState getParseErrorState() {
            return ParseErrorState(m_errors.size());
        }

        /**
         * Sets the parse error state.
         * @param state the error state to set.
         */
        void setParseErrorState(const ParseErrorState& state) {
            m_errors.resize(state.getErrorsSize());
        }

        /**
         * Returns the complete state of the context (parse state and error state).
         * The match state is not preserved, since this call is only meant to be used
         * from parsers that only check and do not do any matches.
         * @return the complete state of the context.
         */
        std::tuple<ParseState, ParseErrorState> getState() {
            return std::make_tuple(getParseState(), getParseErrorState());
        }

        /**
         * Sets the complete state of the context.
         * @param state state to set.
         */
        void setState(const std::tuple<ParseState, ParseErrorState>& state) {
            setParseState(std::get<0>(state));
            setParseErrorState(std::get<1>(state));
        }

        /**
         * Checks if the given rule is currently left-recursive.
         * @param rule rule to check.
         * @return true if the rule is currently left-recursive, false otherwise.
         */
        bool isLeftRecursiveRule(Rule* rule) const {
            auto it = m_rulePositions.find(rule);
            return it != m_rulePositions.end() && !it->second.empty() && it->second.back() == m_currentPosition ? true : false;
        }

        /**
         * Pushes the current parsing position for the given rule, in order to later recognize left recursion for this rule.
         * @param rule rule to push the current parsing position of.
         */
        void pushRulePosition(Rule* rule) {
            m_rulePositions[rule].push_back(m_currentPosition);
        }

        /**
         * Pops the rule position for the given rule.
         * @param rule rule to pop the position of.
         */
        void popRulePosition(Rule* rule) {
            m_rulePositions[rule].pop_back();
        }

        /**
         * Returns the left recursion state of the given rule.
         * @param rule the rule to get the left-recursion state of.
         * @return the left-recursion state of the given rule.
         */
        LeftRecursion::State getLeftRecursionRuleState(Rule* rule) {
            auto it = m_ruleStates.find(rule);
            return it != m_ruleStates.end() ? it->second : LeftRecursion::State::Init;
        }

        /**
         * Sets the left recursion state of the given rule.
         * @param rule the rule to set the left recursion state of.
         * @param state the left recursion state of the rule.
         */
        void setLeftRecursionRuleState(Rule* rule, LeftRecursion::State state) {
            m_ruleStates[rule] = state;
        }

        /**
         * Returns the current matches.
         * @return the current matches.
         */
        const MatchContainer& getMatches() const {
            return m_matches;
        }

        /**
         * Adds a match.
         * @param id the match id.
         * @param startPosition start position of the match, into the source.
         * @param endPosition end position (non-inclusive) of the match, into the source.
         * @param childCount number of children to pop from the current match stack and set as children for the new match.
         * @exception std::invalid_argument thrown if the child count is greater than the number of the existing matches.
         */
        void addMatch(const MatchID& id, const Iterator& startPosition, const Iterator& endPosition, size_t childCount) {
            if (childCount > m_matches.size()) {
                throw std::invalid_argument("ParseContext::addMatch: child match count greater than existing match count.");
            }
            MatchContainer children(m_matches.end() - childCount, m_matches.end());
            Match match(id, startPosition, endPosition, std::move(children));
            m_matches.resize(m_matches.size() - childCount);
            m_matches.push_back(std::move(match));
        }

        /**
         * Returns the appropriate start position and match count for a match, 
         * depending on the left recursion state of the context.
         * @return a tuple that holds the start position and match size for the current match.
         */
        std::tuple<Iterator, size_t> getMatchStart() const {
            if (m_leftRecursionMatches.empty() || m_currentPosition > m_leftRecursionMatches.back().rejectPosition) {
                return std::make_tuple(m_currentPosition, m_matches.size());
            }
            return std::make_tuple(m_leftRecursionMatches.back().startPosition, m_leftRecursionMatches.back().startMatchIndex);
        }

        /**
         * Returns the end position and match count for the current match.
         * @return the end position and match count for the current match.
         */
        std::tuple<Iterator, size_t> getMatchEnd() const {
            return std::make_tuple(m_currentPosition, m_matches.size());
        }

        /**
         * Pushes a new match state for left recursion into an internal stack.
         * @param startPosition start position of the match.
         * @param startMatchIndex index of first match into the internal array of matches, for left recursion.
         * @param rejectPosition position into the source that was rejected due to left recursion.
         */
        void pushLeftRecursionMatchState(const Iterator& startPosition, size_t startMatchIndex, const Iterator& rejectPosition) {
            m_leftRecursionMatches.push_back(LeftRecursionMatchState{startPosition, startMatchIndex, rejectPosition});
        }

        /**
         * Pops the top left recursion match state.
         * If there is none, the behavior is undefined.
         */
        void popLeftRecursionMatchState() {
            m_leftRecursionMatches.pop_back();
        }

    private:
        struct LeftRecursionMatchState {
            Iterator startPosition;
            size_t startMatchIndex;
            Iterator rejectPosition;
        };

        Iterator m_currentPosition;
        Iterator m_endPosition;
        Iterator m_furthestUnparsedPosition;
        ParseErrorContainer m_errors;
        std::map<Rule*, std::vector<Iterator>> m_rulePositions;
        std::map<Rule*, LeftRecursion::State> m_ruleStates;
        MatchContainer m_matches;
        std::vector<LeftRecursionMatchState> m_leftRecursionMatches;
    };


} //namespace parserlib::core


#endif //PARSERLIB_CORE_PARSECONTEXT_HPP
