#ifndef PARSERLIB_PARSECONTEXT_HPP
#define PARSERLIB_PARSECONTEXT_HPP


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


namespace parserlib {


    template <class ParseContext> class Rule;


    template <class MatchId = int, class Source = SourceString<>>
    class ParseContext {
    public:
        typedef Source Source;
        typedef typename Source::const_iterator Iterator;
        typedef ParseState<Source> ParseState;
        typedef ParseError<Source> ParseError;
        typedef std::vector<ParseError> ParseErrorContainer;
        typedef Rule<ParseContext<MatchId, Source>> Rule;
        typedef Match<MatchId, Source> Match;
        typedef MatchContainer<MatchId, Source> MatchContainer;

        ParseContext(Source& src)
            : m_currentPosition(src.begin())
            , m_endPosition(src.end())
            , m_committedErrorsSize(0)
        {
        }

        const Iterator& getCurrentPosition() const {
            return m_currentPosition;
        }

        const Iterator& getEndPosition() const {
            return m_endPosition;
        }

        bool isEndPosition() const {
            return m_currentPosition == m_endPosition;
        }

        void incrementPosition() {
            ++m_currentPosition;
        }

        void incrementPosition(size_t size) {
            m_currentPosition += size;
        }

        ParseState getParseState() const {
            return ParseState(m_currentPosition, m_matches.size(), m_leftRecursionMatches.size());
        }

        void setParseState(const ParseState& state) {
            m_currentPosition = state.getPosition();
            m_matches.resize(state.getMatchCount());
            m_leftRecursionMatches.resize(state.getLeftRecursionMatchStateCount());
        }

        const ParseErrorContainer& getErrors() const {
            return m_errors;
        }

        void setError(const ParseError& error) {
            if (m_errors.size() == m_committedErrorsSize) {
                m_errors.push_back(error);
            }
            else if (error.getPosition() > m_errors.back().getPosition()) {
                m_errors.back() = error;
            }
        }

        template <class Error>
        void setError(Error errorId) {
            setError(ParseError(static_cast<int>(errorId), m_currentPosition));
        }

        void commitErrors() {
            m_committedErrorsSize = m_errors.size();
        }

        ParseErrorState getParseErrorState() {
            return ParseErrorState(m_errors.size(), m_committedErrorsSize);
        }

        void setParseErrorState(const ParseErrorState& state) {
            m_errors.resize(state.getErrorsSize());
            m_committedErrorsSize = state.getCommittedErrorsSize();
        }

        std::tuple<ParseState, ParseErrorState> getState() {
            return std::make_tuple(getParseState(), getParseErrorState());
        }

        void setState(const std::tuple<ParseState, ParseErrorState>& state) {
            setParseState(std::get<0>(state));
            setParseErrorState(std::get<1>(state));
        }

        bool isLeftRecursiveRule(Rule* rule) const {
            auto it = m_rulePositions.find(rule);
            return it != m_rulePositions.end() && !it->second.empty() && it->second.back() == m_currentPosition ? true : false;
        }

        void pushRulePosition(Rule* rule) {
            m_rulePositions[rule].push_back(m_currentPosition);
        }

        void popRulePosition(Rule* rule) {
            m_rulePositions[rule].pop_back();
        }

        LeftRecursion::State getLeftRecursionRuleState(Rule* rule) {
            auto it = m_ruleStates.find(rule);
            return it != m_ruleStates.end() ? it->second : LeftRecursion::State::Init;
        }

        void setLeftRecursionRuleState(Rule* rule, LeftRecursion::State state) {
            m_ruleStates[rule] = state;
        }

        const MatchContainer& getMatches() const {
            return m_matches;
        }

        void addMatch(const MatchId& id, const Iterator& startPosition, const Iterator& endPosition, size_t childCount) {
            if (childCount > m_matches.size()) {
                throw std::invalid_argument("ParseContext::addMatch: child match count greater than existing match count.");
            }
            MatchContainer children(m_matches.end() - childCount, m_matches.end());
            Match match(id, startPosition, endPosition, std::move(children));
            m_matches.resize(m_matches.size() - childCount);
            m_matches.push_back(std::move(match));
        }

        std::tuple<Iterator, size_t> getMatchStart() const {
            if (m_leftRecursionMatches.empty() || m_currentPosition > m_leftRecursionMatches.back().rejectPosition) {
                return std::make_tuple(m_currentPosition, m_matches.size());
            }
            return std::make_tuple(m_leftRecursionMatches.back().startPosition, m_leftRecursionMatches.back().startMatchIndex);
        }

        std::tuple<Iterator, size_t> getMatchEnd() const {
            return std::make_tuple(m_currentPosition, m_matches.size());
        }

        void pushLeftRecursionMatchState(const Iterator& startPosition, size_t startMatchIndex, const Iterator& rejectPosition) {
            m_leftRecursionMatches.push_back(LeftRecursionMatchState{startPosition, startMatchIndex, rejectPosition});
        }

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
        size_t m_committedErrorsSize;
        ParseErrorContainer m_errors;
        std::map<Rule*, std::vector<Iterator>> m_rulePositions;
        std::map<Rule*, LeftRecursion::State> m_ruleStates;
        MatchContainer m_matches;
        std::vector<LeftRecursionMatchState> m_leftRecursionMatches;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSECONTEXT_HPP
