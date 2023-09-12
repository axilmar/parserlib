#ifndef PARSERLIB_PARSERCONTEXT_HPP
#define PARSERLIB_PARSERCONTEXT_HPP


#include <string>
#include <vector>
#include <map>
#include "Match.hpp"
#include "TreeMatchException.hpp"
#include "RuleState.hpp"
#include "SourcePosition.hpp"
#include "LineCountingSourcePosition.hpp"
#include "Error.hpp"


namespace parserlib {


    template <class ParseContextType> class Rule;


    /**
     * A parser context implementation.
     * @param SourceType container with source data; 
     *  must have an STL container interface;
     *  must outlive the parser context;
     *  must be immutable while being used by a parser context.
     * @param MatchIdType id to apply to a match.
     * @param PositionType type of source position.
     */
    template <class SourceType_ = std::string, class MatchIdType_ = std::string, class SourcePositionType_ = SourcePosition<SourceType_>>
    class ParseContext {
    public:
        /**
         * Source type.
         */
        using SourceType = SourceType_;

        /**
         * Match id type.
         */
        using MatchIdType = MatchIdType_;

        /**
         * source position type.
         */
        using PositionType = SourcePositionType_;

        /**
         * this type.
         */
        using ThisType = ParseContext<SourceType, MatchIdType, PositionType>;

        /**
         * Associated rule type.
         */
        using RuleType = Rule<ThisType>;

        /**
         * Associated rule state type. 
         */
        using RuleStateType = RuleState<ThisType>;

        /**
         * Match type.
         */
        using MatchType = Match<SourceType, MatchIdType, PositionType>;

        /**
         * Current parser state. 
         */
        class State {
        public:
            /**
             * The default constructor.
             * The state is invalid and shall not be used until initialized.
             */
            State() {
            }

            /**
             * Returns the current source position.
             * @return the current source position.
             */
            const PositionType& sourcePosition() const {
                return m_sourcePosition;
            }

            /**
             * Returns the number of matches.
             * @return the number of matches.
             */
            size_t matchCount() const {
                return m_matchCount;
            }

        private:
            const PositionType m_sourcePosition;
            const size_t m_matchCount;

            //constructor
            State(const PositionType& position, const size_t matchCount) 
                : m_sourcePosition(position), m_matchCount(matchCount)
            {
            }

            friend ThisType;
        };

        /**
         * Constructor.
         * @param src source.
         */
        ParseContext(const SourceType& src)
            : m_sourcePosition(src.begin(), src.end())
        {
        }

        /**
         * Returns the current state.
         * @return the current state.
         */
        State state() const {
            return State(m_sourcePosition, m_matches.size());
        }

        /**
         * Sets the current state.
         * @param state state.
         */
        void setState(const State& state) {
            m_sourcePosition = state.sourcePosition();
            m_matches.resize(state.matchCount());
        }

        /**
         * Returns the current source position.
         * @return the current source position.
         */
        const PositionType& sourcePosition() const {
            return m_sourcePosition;
        }

        /**
         * Checks if the element at the current source position equals the given value.
         * @param value value to compare to the current element.
         * @return true if current element equals given value, false otherwise.
         */
        template <class T>
        bool sourcePositionContains(const T& value) const {
            return m_sourcePosition.contains(value);
        }

        /**
         * Checks if the element at the current source position is within the given range of values.
         * @param minValue lowest value to compare to the current element.
         * @param maxValue highest value to compare to the current element.
         * @return true if current element is within the given range, false otherwise.
         */
        template <class T>
        bool sourcePositionContains(const T& minValue, const T& maxValue) const {
            return m_sourcePosition.contains(minValue, maxValue);
        }

        /**
         * Checks if the element at the current source position is one of the given values.
         * @param values values to check.
         * @return true if current element is within the given values, false otherwise.
         */
        template <class T, class Alloc>
        bool sourcePositionContains(const std::vector<T, Alloc>& values) const {
            return m_sourcePosition.contains(values);
        }

        /**
         * Checks if the given null-terminated string can be recognized at current position.
         * @param str string to check.
         * @return true if string is recognized, false otherwise.
         */
        template <class T>
        bool sourcePositionContains(const T* str) const {
            return m_sourcePosition.contains(str);
        }

        /**
         * Increments the source position. 
         */
        void incrementSourcePosition() {
            m_sourcePosition.increment();
        }

        /**
         * Increases the source position by the given count.
         * @param count number of places to increase the source position.
         */
        void increaseSourcePosition(size_t count) {
            m_sourcePosition.increase(count);
        }

        /**
         * Returns the end of the source.
         * @return the end of the source.
         */
        const typename SourceType::const_iterator& sourceEnd() const {
            return m_sourcePosition.end();
        }

        /**
         * Checks if the source has ended.
         * @return true if there is no more source to parse, false otherwise.
         */
        bool sourceEnded() const {
            return m_sourcePosition == m_sourcePosition.end();
        }
       
        /**
         * Returns the current matches.
         * @return the current matches.
         */
        const std::vector<MatchType>& matches() const {
            return m_matches;
        }

        /**
         * Adds a match.
         * @param id match id.
         * @param begin begin position into the source.
         * @param end end position into the source.
         */
        void addMatch(const MatchIdType& id, const PositionType& begin, const PositionType& end) {
            m_matches.push_back(MatchType(id, begin, end));
        }

        /**
         * Adds a match, moving the given number of matches to children.
         * @param id match id.
         * @param begin begin position into the source.
         * @param end end position into the source.
         * @param childCount number of matches to add to the parent.
         * @exception TreeMatchException thrown if the given number of children does not exist in the current match table.
         */
        void addMatch(const MatchIdType& id, const PositionType& begin, const PositionType& end, size_t childCount) {
            if (childCount > m_matches.size()) {
                throw TreeMatchException<ThisType>(*this);
            }
            MatchType m(id, begin, end, std::vector<MatchType>(m_matches.end() - childCount, m_matches.end()));
            m_matches.resize(m_matches.size() - childCount);
            m_matches.push_back(std::move(m));
        }

        /**
         * Returns the existing rule state for the given rule.
         * @param rule rule to get the rule state of.
         * @return the rule state for the rule.
         * @exception std::runtime_error thrown if there is no rule state for the given rule.
         */
        const RuleStateType& ruleState(const RuleType& rule) const {
            const auto it = m_ruleStates.find(rule.this_());
            return it != m_ruleStates.end() ? it->second : throw std::runtime_error("No rule state for the given rule exists.");
        }

        /**
         * Returns the existing or a new rule state for the given rule.
         * @param rule rule to get the rule state of.
         * @return the rule state for the rule.
         */
        RuleStateType& ruleState(const RuleType& rule) {
            const auto it1 = m_ruleStates.find(rule.this_());
            if (it1 != m_ruleStates.end()) {
                return it1->second;
            }
            const auto [it2, ok] = m_ruleStates.emplace(rule.this_(), RuleStateType(PositionType(m_sourcePosition.end(), m_sourcePosition.end())));
            return it2->second;
        }

        /**
         * Error state.
         */
        class ErrorState {
        public:

        private:
            size_t m_errorCount;

            ErrorState(size_t errorCount) : m_errorCount(errorCount) {
            }

            friend ThisType;
        };

        /**
         * Returns the current error state.
         * @return the current error state.
         */
        ErrorState errorState() const {
            return { m_errors.size() };
        }

        /**
         * Sets the current error state.
         * @param es the error state to set the current error state from.
         */
        void setErrorState(const ErrorState& es) {
            m_errors.resize(std::max(es.m_errorCount, m_committedErrorCount));
        }

        /**
         * Returns the current list of errors.
         * @return the current list of errors.
         */
        const ErrorContainer<PositionType>& errors() const {
            return m_errors;
        }
             
        /**
         * Adds an error.
         * 
         * An error is added only if:
         *  - there is no error currently added.
         *  - there is no uncommitted error.
         *  - the new error happened in a position that is greater than the last uncommitted error.
         * 
         * @param pos position that the error happened at.
         * @param ecf error creation function; it allows the creation of the error message only if needed.
         */
        template <class ErrorCreationFunc> void addError(const PositionType& pos, const ErrorCreationFunc& ecf) {
            if (m_errors.size() == m_committedErrorCount) {
                m_errors.push_back(ecf());
            }
            else if (pos > m_errors.back().position()) {
                m_errors.back() = ecf();
            }
        }

        /**
         * Commits the current set of errors.
         */
        void commitErrors() {
            m_committedErrorCount = m_errors.size();
        }

    private:
        PositionType m_sourcePosition;
        std::vector<MatchType> m_matches;
        std::map<const RuleType*, RuleStateType> m_ruleStates;
        ErrorContainer<PositionType> m_errors;
        size_t m_committedErrorCount{ 0 };
    };


} //namespace parserlib


#endif //PARSERLIB_PARSERCONTEXT_HPP
