#ifndef PARSERLIB_RULESTATE_HPP
#define PARSERLIB_RULESTATE_HPP


namespace parserlib {


    /**
     * A rule's state.
     * @param ParseContextType type of parse context.
     */
    template <class ParseContextType> class RuleState {
    public:
        /**
         * Constructor.
         * @param position initial position.
         */
        RuleState(const typename ParseContextType::PositionType& position = {}) 
            : m_position(position) {
        }

        /**
         * Returns the last position into source that the rule was called to parse.
         * @param returns the last known position for the rule.
         */
        const typename ParseContextType::PositionType& position() const {
            return m_position;
        }

        /**
         * Sets the parse position of the rule.
         * @param position the parse position.
         */
        void setPosition(const typename ParseContextType::PositionType& position) {
            m_position = position;
        }

        /**
         * Returns the left recursion flag.
         * @return the left recursion flag.
         */
        bool leftRecursion() const {
            return m_leftRecursion;
        }

        /**
         * Sets the left recursion flag.
         * @param lr left recursion flag.
         */
        void setLeftRecursion(bool lr) {
            m_leftRecursion = lr;
        }

    private:
        typename ParseContextType::PositionType m_position;
        bool m_leftRecursion{ false };
    };


} //namespace parserlib


#endif //PARSERLIB_RULESTATE_HPP
