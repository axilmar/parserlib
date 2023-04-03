#ifndef PARSERLIB_PARSERCONTEXT_HPP
#define PARSERLIB_PARSERCONTEXT_HPP


#include <string>
#include <vector>


namespace parserlib {


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
        using Position = typename SourceType::const_iterator;

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

        private:
            const MatchIdType m_id{};
            typename SourceType::const_iterator m_begin;
            typename SourceType::const_iterator m_end;

            //internal constructor
            Match(const MatchIdType& id, const typename SourceType::const_iterator& begin, const typename SourceType::const_iterator& end) 
                : m_id(id), m_begin(begin), m_end(end) {
            }

            friend class ParseContext<SourceType, MatchIdType>;
        };

        /**
         * Current parser state. 
         */
        class State {
        public:
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

    private:
        typename SourceType::const_iterator m_sourceIt;
        const typename SourceType::const_iterator m_sourceEnd;
        std::vector<Match> m_matches;
    };


} //namespace parserlib


#endif //PARSERLIB_PARSERCONTEXT_HPP
