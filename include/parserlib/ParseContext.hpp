#ifndef PARSERLIB_PARSERCONTEXT_HPP
#define PARSERLIB_PARSERCONTEXT_HPP


#include <string>
#include <vector>
#include <map>
#include "TreeMatchException.hpp"
#include "RuleState.hpp"


namespace parserlib {


    template <class ParseContextType> class Rule;


    /**
     * A parser context implementation.
     * @param SourceType container with source data; 
     *  must have an STL container interface;
     *  must outlive the parser context;
     *  must be immutable while being used by a parser context.
     * @param MatchIdType id to apply to a match.
     */
    template <class SourceType = std::string, class MatchIdType = std::string> class ParseContext {
    public:
        /**
         * Source type.
         */
        using Source = SourceType;

        /**
         * Parsing position type.
         */
        using PositionType = typename SourceType::const_iterator;

        /**
         * this type.
         */
        using ThisType = ParseContext<SourceType, MatchIdType>;

        /**
         * Associated rule type.
         */
        using RuleType = Rule<ThisType>;

        /**
         * Associated rule state type. 
         */
        using RuleStateType = RuleState<ThisType>;

        /**
         * A successful parse. 
         */
        class Match {
        public:
            /**
             * The default constructor.
             * No member is initialized.
             */
            Match() {
            }

            /**
             * Returns the id of the match.
             * @return the id of the match.
             */
            const MatchIdType& id() const {
                return m_id;
            }
            
            /**
             * Returns the start of the match within the source.
             * @return the start of the match within the source.
             */
            const typename SourceType::const_iterator& begin() const {
                return m_begin;
            }

            /**
             * Returns the end of the match within the source.
             * @return the end of the match within the source.
             */
            const typename SourceType::const_iterator& end() const {
                return m_end;
            }

            /**
             * Returns the parsed content.
             * @return the parsed content.
             */
            SourceType content() const {
                return SourceType(m_begin, m_end);
            }

            /**
             * Returns the children matches.
             * @return the children matches.
             */
            const std::vector<Match>& children() const {
                return m_children;
            }

        private:
            const MatchIdType m_id{};
            typename SourceType::const_iterator m_begin;
            typename SourceType::const_iterator m_end;
            std::vector<Match> m_children;

            //internal constructor
            Match(const MatchIdType& id, 
                  const typename SourceType::const_iterator& begin, 
                  const typename SourceType::const_iterator& end, 
                  std::vector<Match>&& children = std::vector<Match>()) 
                : m_id(id), m_begin(begin), m_end(end), m_children(children) {
            }

            friend class ParseContext<SourceType, MatchIdType>;
        };

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
            const typename SourceType::const_iterator& sourcePosition() const {
                return m_sourceIt;
            }

            /**
             * Returns the number of matches.
             * @return the number of matches.
             */
            size_t matchCount() const {
                return m_matchCount;
            }

        private:
            typename SourceType::const_iterator m_sourceIt;
            typename size_t m_matchCount;

            //constructor
            State(const typename SourceType::const_iterator& sourceIt, const size_t matchCount) 
                : m_sourceIt(sourceIt), m_matchCount(matchCount) {
            }

            friend class ParseContext<SourceType, MatchIdType>;
        };

        /**
         * Constructor.
         * @param src source.
         */
        ParseContext(const SourceType& src)
            : m_sourceIt(src.begin()), m_sourceEnd(src.end()) {
        }

        /**
         * Returns the current state.
         * @return the current state.
         */
        State state() const {
            return State(m_sourceIt, m_matches.size());
        }

        /**
         * Sets the current state.
         * @param state state.
         */
        void setState(const State& state) {
            m_sourceIt = state.sourcePosition();
            m_matches.resize(state.matchCount());
        }

        /**
         * Returns the current source position.
         * @return the current source position.
         */
        const typename SourceType::const_iterator& sourcePosition() const {
            return m_sourceIt;
        }

        /**
         * Returns the current matches.
         * @return the current matches.
         */
        const std::vector<Match>& matches() const {
            return m_matches;
        }

        /**
         * Adds a match.
         * @param id match id.
         * @param begin begin position into the source.
         * @param end end position into the source.
         */
        void addMatch(const MatchIdType& id, const typename SourceType::const_iterator& begin, const typename SourceType::const_iterator& end) {
            m_matches.push_back(Match(id, begin, end));
        }

        /**
         * Adds a match, moving the given number of matches to children.
         * @param id match id.
         * @param begin begin position into the source.
         * @param end end position into the source.
         * @param childCount number of matches to add to the parent.
         * @exception TreeMatchException thrown if the given number of children does not exist in the current match table.
         */
        void addMatch(const MatchIdType& id, const typename SourceType::const_iterator& begin, const typename SourceType::const_iterator& end, size_t childCount) {
            if (childCount > m_matches.size()) {
                throw TreeMatchException<ParseContext<SourceType, MatchIdType>>(*this);
            }
            Match m(id, begin, end, std::vector<Match>(m_matches.end() - childCount, m_matches.end()));
            m_matches.resize(m_matches.size() - childCount);
            m_matches.push_back(std::move(m));
        }

        /**
         * Returns the end position of the source.
         * @return the end position of the source.
         */
        const typename SourceType::const_iterator& sourceEndPosition() const {
            return m_sourceEnd;
        }

        /**
         * Increments the source position.
         * No check for beyond end of source is done.
         */
        void incrementSourcePosition() {
            ++m_sourceIt;
        }

        /**
         * Increases the source position by a specific count.
         * @param count number of positions to increment the source position.
         * No check for beyond end of source is done.
         */
        void increaseSourcePosition(size_t count) {
            m_sourceIt += count;
        }

        /**
         * Checks if the source has ended.
         * @return true if there is no more source to parse, false otherwise.
         */
        bool sourceEnded() const {
            return m_sourceIt == m_sourceEnd;
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
            const auto [it2, ok] = m_ruleStates.emplace(rule.this_(), RuleStateType(m_sourceEnd));
            return it2->second;
        }

    private:
        typename SourceType::const_iterator m_sourceIt;
        const typename SourceType::const_iterator m_sourceEnd;
        std::vector<Match> m_matches;
        std::map<const RuleType*, RuleStateType> m_ruleStates;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSERCONTEXT_HPP
